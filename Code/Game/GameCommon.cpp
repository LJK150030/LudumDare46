#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


float CalcAverageTick(const float new_tick)
{
	g_tickSum -= g_tickList[g_tickIndex];
	g_tickSum += new_tick;
	g_tickList[g_tickIndex] = new_tick;
	if (++g_tickIndex == g_maxTickSample)    /* inc buffer index */
	{
		g_tickIndex = 0;
	}

	/* return average */
	return(g_tickSum / static_cast<float>(g_maxTickSample));
}

void DrawLine(const Vec2& start, const Vec2& end, float thickness, const Rgba& tint)
{
	std::vector<Vertex_PCU> lineVerts;
	AddVertsForLine2D(lineVerts, start, end, thickness, tint);
	g_theRenderer->DrawVertexArray(lineVerts);
}
