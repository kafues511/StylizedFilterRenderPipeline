// Copyright 2024 kafues511. All Rights Reserved.

#include "StylizedFilterViewExtension.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterialInputs.h"
#include "ScenePrivate.h"
#include "SystemTextures.h"
#include "SceneTextureParameters.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "StylizedFilterSubsystem.h"

DECLARE_GPU_STAT(StylizedFilter);

class FSNNPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSNNPS);
	SHADER_USE_PARAMETER_STRUCT(FSNNPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureShaderParameters, SceneTextures)
		SHADER_PARAMETER(int32, KernelHalfSize)
		SHADER_PARAMETER(float, InvWeight)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) || IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
	}
};

class FKuwaharaPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FKuwaharaPS);
	SHADER_USE_PARAMETER_STRUCT(FKuwaharaPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureShaderParameters, SceneTextures)
		SHADER_PARAMETER(float, InvWeight)
		SHADER_PARAMETER_ARRAY(FIntVector4, IterParams, [4])
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) || IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
	}
};

IMPLEMENT_GLOBAL_SHADER(FSNNPS,      "/Plugin/StylizedFilterRenderPipeline/Private/SNN.usf",      "MainPS", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FKuwaharaPS, "/Plugin/StylizedFilterRenderPipeline/Private/Kuwahara.usf", "MainPS", SF_Pixel);

FStylizedFilterViewExtension::FStylizedFilterViewExtension(const FAutoRegister& AutoRegister, UStylizedFilterSubsystem* InStylizedFilterSubsystem)
	: FSceneViewExtensionBase(AutoRegister)
	, StylizedFilterSubsystem(InStylizedFilterSubsystem)
{
}

void FStylizedFilterViewExtension::Invalidate()
{
	StylizedFilterSubsystem = nullptr;
}

void FStylizedFilterViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	check(IsInGameThread());

	if (!IsValid(StylizedFilterSubsystem))
	{
		return;
	}

	FinalStylizedFilterSettings = StylizedFilterSubsystem->GetStylizedFilterSettings();
}

void FStylizedFilterViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	if (!FinalStylizedFilterSettings.IsValid())
	{
		return;
	}

	Inputs.Validate();

	const FIntRect PrimaryViewRect = static_cast<const FViewInfo&>(View).ViewRect;

	// Scene color is updated incrementally through the post process pipeline.
	FScreenPassTexture SceneColor((*Inputs.SceneTextures)->SceneColorTexture, PrimaryViewRect);

	RDG_EVENT_SCOPE(GraphBuilder, "StylizedFilter");
	RDG_GPU_STAT_SCOPE(GraphBuilder, StylizedFilter);

	const FScreenPassTextureViewport Viewport(SceneColor);

	FRDGTextureRef OutputTexture;
	{
		FRDGTextureDesc OutputTextureDesc = SceneColor.Texture->Desc;
		OutputTextureDesc.Reset();
		OutputTextureDesc.Flags |= TexCreate_RenderTargetable | TexCreate_ShaderResource;
		OutputTexture = GraphBuilder.CreateTexture(OutputTextureDesc, TEXT("StylizedFilter.Output"));
	}

	// Stylized Filter Pass (SNN)
	if (FinalStylizedFilterSettings.FilterType == EStylizedFilterType::SNN)
	{
		const int32 KernelFullSize = FinalStylizedFilterSettings.Kernel + ((FinalStylizedFilterSettings.Kernel % 2 == 0) ? 1 : 0);
		const int32 KernelHalfSize = FMath::FloorToInt32(static_cast<float>(KernelFullSize) * 0.5f);

		TShaderMapRef<FScreenVS> VertexShader(static_cast<const FViewInfo&>(View).ShaderMap);
		TShaderMapRef<FSNNPS> PixelShader(static_cast<const FViewInfo&>(View).ShaderMap);

		FSNNPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FSNNPS::FParameters>();
		PassParameters->View = View.ViewUniformBuffer;
		PassParameters->SceneTextures = GetSceneTextureShaderParameters(Inputs.SceneTextures);
		PassParameters->KernelHalfSize = KernelHalfSize;
		PassParameters->InvWeight = 1.0f / (1.0f + KernelHalfSize + KernelFullSize * KernelHalfSize);
		PassParameters->RenderTargets[0] = FRenderTargetBinding(OutputTexture, ERenderTargetLoadAction::EClear);

		const FScreenPassTexture BlackDummy(GSystemTextures.GetBlackDummy(GraphBuilder));

		// This gets passed in whether or not it's used.
		GraphBuilder.RemoveUnusedTextureWarning(BlackDummy.Texture);

		AddDrawScreenPass(
			GraphBuilder,
			RDG_EVENT_NAME("SNN (PS)"),
			View,
			Viewport,
			Viewport,
			VertexShader,
			PixelShader,
			TStaticBlendState<>::GetRHI(),
			TStaticDepthStencilState<false, CF_Always>::GetRHI(),
			PassParameters);
	}
	// Stylized Filter Pass (Kuwahara)
	else if (FinalStylizedFilterSettings.FilterType == EStylizedFilterType::Kuwahara)
	{
		const int32 KernelFullSize = FinalStylizedFilterSettings.Kernel + ((FinalStylizedFilterSettings.Kernel % 2 == 0) ? 1 : 0);
		const int32 KernelHalfSize = FMath::FloorToInt32(static_cast<float>(KernelFullSize) * 0.5f);

		TShaderMapRef<FScreenVS> VertexShader(static_cast<const FViewInfo&>(View).ShaderMap);
		TShaderMapRef<FKuwaharaPS> PixelShader(static_cast<const FViewInfo&>(View).ShaderMap);

		FKuwaharaPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FKuwaharaPS::FParameters>();
		PassParameters->View = View.ViewUniformBuffer;
		PassParameters->SceneTextures = GetSceneTextureShaderParameters(Inputs.SceneTextures);
		PassParameters->InvWeight = 1.0f / ((KernelHalfSize + 1) * (KernelHalfSize + 1));
		PassParameters->IterParams[0] = FIntVector4(-KernelHalfSize,              0, -KernelHalfSize,              0);
		PassParameters->IterParams[1] = FIntVector4(-KernelHalfSize,              0,               0, KernelHalfSize);
		PassParameters->IterParams[2] = FIntVector4(              0, KernelHalfSize, -KernelHalfSize,              0);
		PassParameters->IterParams[3] = FIntVector4(              0, KernelHalfSize,               0, KernelHalfSize);

		PassParameters->RenderTargets[0] = FRenderTargetBinding(OutputTexture, ERenderTargetLoadAction::EClear);

		const FScreenPassTexture BlackDummy(GSystemTextures.GetBlackDummy(GraphBuilder));

		// This gets passed in whether or not it's used.
		GraphBuilder.RemoveUnusedTextureWarning(BlackDummy.Texture);

		AddDrawScreenPass(
			GraphBuilder,
			RDG_EVENT_NAME("Kuwahara (PS)"),
			View,
			Viewport,
			Viewport,
			VertexShader,
			PixelShader,
			TStaticBlendState<>::GetRHI(),
			TStaticDepthStencilState<false, CF_Always>::GetRHI(),
			PassParameters);
	}

	// Copy Pass
	{
		AddCopyTexturePass(GraphBuilder, OutputTexture, SceneColor.Texture);
	}
}
