/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/

/*******************************************************************************************
*	Code and amendments by s0lly														   *
*	https://www.youtube.com/c/s0lly							                               *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
********************************************************************************************/

#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Vec2.h"

class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c );

	void PutPixelWithAlphaBlend(int x, int y, Color c, float a)
	{
		Color existingColor = pSysBuffer[Graphics::ScreenWidth * y + x];
		PutPixel(x, y,
			{	(unsigned char)((float)existingColor.GetR() * (1.0f - a) + (float)c.GetR() * (a)),
				(unsigned char)((float)existingColor.GetG() * (1.0f - a) + (float)c.GetG() * (a)),
				(unsigned char)((float)existingColor.GetB() * (1.0f - a) + (float)c.GetB() * (a)) });
	}

	void DrawRect(Vec2 loc, int length, int height, Color c, float alpha = 1.0f)
	{
		for (int j = (int)loc.y; j < (int)loc.y + height; j++)
		{
			if (j >= 0 && j < ScreenHeight)
			{
				for (int i = (int)loc.x; i < (int)loc.x + length; i++)
				{
					if (i >= 0 && i < ScreenWidth)
					{
						PutPixelWithAlphaBlend(i, j, c, alpha);
					}
				}
			}
		}
	}

	void DrawRectWithinCircle(Vec2 loc, int length, int height, Color c, Vec2 circleLoc, float circleRadius, float alpha = 1.0f)
	{
		for (int j = (int)loc.y; j < (int)loc.y + height; j++)
		{
			if (j >= 0 && j < ScreenHeight)
			{
				for (int i = (int)loc.x; i < (int)loc.x + length; i++)
				{
					if (i >= 0 && i < ScreenWidth)
					{
						if ((Vec2((float)i, (float)j) - circleLoc).GetMagnitudeSqrd() < (circleRadius * circleRadius))
						{
							PutPixelWithAlphaBlend(i, j, c, alpha);
						}	
					}
				}
			}
		}
	}
	
	void DrawCircle(Vec2 loc, float radius, Color c, float alpha = 1.0f)
	{
		// Initial calculations ensure that only thevisible part of each circle is drawn

		int left = (int)(loc.x - radius);
		left = left < 0 ? 0 : left;

		int right = (int)(loc.x + radius) + 1;
		right = right >= ScreenWidth ? ScreenWidth : right;

		int top = (int)(loc.y - radius);
		top = top < 0 ? 0 : top;

		int bottom = (int)(loc.y + radius) + 1;
		bottom = bottom >= ScreenHeight ? ScreenHeight : bottom;

		float radiusSqrd = radius * radius;

		for (int j = top; j < bottom; j++)
		{
			float distSqrdY = ((float)j - loc.y) * ((float)j - loc.y);

			for (int i = left; i < right; i++)
			{
				float distSqrdX = ((float)i - loc.x) * ((float)i - loc.x);
				float distSqrdTotal = distSqrdX + distSqrdY;

				if (distSqrdTotal < radiusSqrd)
				{
					PutPixelWithAlphaBlend(i, j, c, alpha);
				}
			}
		}
	}

	void DrawCircleWithinCircle(Vec2 loc, float radius, Color c, Vec2 outerLoc, float outerRadius, float alpha = 1.0f)
	{
		// Initial calculations ensure that only thevisible part of each circle is drawn

		int left = (int)(loc.x - radius);
		left = left < 0 ? 0 : left;

		int right = (int)(loc.x + radius) + 1;
		right = right >= ScreenWidth ? ScreenWidth : right;

		int top = (int)(loc.y - radius);
		top = top < 0 ? 0 : top;

		int bottom = (int)(loc.y + radius) + 1;
		bottom = bottom >= ScreenHeight ? ScreenHeight : bottom;

		float radiusSqrd = radius * radius;

		for (int j = top; j < bottom; j++)
		{
			float distSqrdY = ((float)j - loc.y) * ((float)j - loc.y);

			for (int i = left; i < right; i++)
			{
				float distSqrdX = ((float)i - loc.x) * ((float)i - loc.x);
				float distSqrdTotal = distSqrdX + distSqrdY;

				if ((distSqrdTotal < radiusSqrd) && (((Vec2((float)i - outerLoc.x, (float)j - outerLoc.y)).GetMagnitude() < outerRadius)))
				{
					PutPixelWithAlphaBlend(i, j, c, alpha);
				}
			}
		}
	}

	void DrawEllipse(Vec2 loc, float radiusWidth, float radiusHeight, Color c, float alpha = 1.0f) // Not working yet!!
	{
		// Initial calculations ensure that only thevisible part of each circle is drawn

		int left = (int)(loc.x - radiusWidth);
		left = left < 0 ? 0 : left;

		int right = (int)(loc.x + radiusWidth) + 1;
		right = right >= ScreenWidth ? ScreenWidth : right;

		int top = (int)(loc.y - radiusHeight);
		top = top < 0 ? 0 : top;

		int bottom = (int)(loc.y + radiusHeight) + 1;
		bottom = bottom >= ScreenHeight ? ScreenHeight : bottom;

		//float radiusSqrd = radius * radius;

		for (int j = top; j < bottom; j++)
		{
			float distSqrdY = ((float)j - loc.y) * ((float)j - loc.y);

			for (int i = left; i < right; i++)
			{
				float distSqrdX = ((float)i - loc.x) * ((float)i - loc.x);
				float distSqrdTotal = distSqrdX + distSqrdY;

				float distanceOfPointToCenterSqrd = Vec2((float)i - loc.x, (float)j - loc.y).GetMagnitudeSqrd();

				float radiusAtPoint = (distSqrdX / distanceOfPointToCenterSqrd) * radiusWidth +
					(distSqrdY / distanceOfPointToCenterSqrd) * radiusHeight;

				if (distSqrdTotal < (radiusAtPoint * radiusAtPoint))
				{
					PutPixelWithAlphaBlend(i, j, c, alpha);
				}
			}
		}
	}

	void DrawCircleWithIncreasingAlphaToEdge(Vec2 loc, float radius, Color c, float startEdge, float startAlpha, float endAlpha = 1.0f)
	{
		// Initial calculations ensure that only thevisible part of each circle is drawn

		int left = (int)(loc.x - radius);
		left = left < 0 ? 0 : left;

		int right = (int)(loc.x + radius) + 1;
		right = right >= ScreenWidth ? ScreenWidth : right;

		int top = (int)(loc.y - radius);
		top = top < 0 ? 0 : top;

		int bottom = (int)(loc.y + radius) + 1;
		bottom = bottom >= ScreenHeight ? ScreenHeight : bottom;

		float radiusSqrd = radius * radius;
		float innerRadiusSqrd = startEdge * startEdge;

		for (int j = top; j < bottom; j++)
		{
			float distSqrdY = ((float)j - loc.y) * ((float)j - loc.y);

			for (int i = left; i < right; i++)
			{
				float distSqrdX = ((float)i - loc.x) * ((float)i - loc.x);
				float distSqrdTotal = distSqrdX + distSqrdY;

				if (distSqrdTotal < radiusSqrd && distSqrdTotal >= innerRadiusSqrd)
				{
					float distTotal = sqrt(distSqrdTotal);
					float check = (distTotal - startEdge) / (radius - startEdge);
					float newAlpha = startAlpha + ((sqrt(distSqrdTotal) - startEdge) / (radius - startEdge)) * (endAlpha - startAlpha);
					PutPixelWithAlphaBlend(i, j, c, newAlpha);
				}
			}
		}
	}

	~Graphics();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
public:
	static constexpr int ScreenWidth = 1600;
	static constexpr int ScreenHeight = 900;
};