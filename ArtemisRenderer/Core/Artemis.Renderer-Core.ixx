module;


export module Artemis.Renderer:Core;

import <d3d12.h>;
import <Windows.h>;
import <assert.h>;

export import "RendererForward.h";

import Artemis.Core;

export namespace ArtemisRenderer::Core
{
	class IRenderer;
	class RendererForward;
}