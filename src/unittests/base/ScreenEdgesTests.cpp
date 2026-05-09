/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "ScreenEdgesTests.h"

#include "base/DirectionTypes.h"
#include "base/ScreenEdges.h"

namespace {

uint32_t bottomSide()
{
  return static_cast<int>(DirectionMask::BottomMask);
}

uint32_t side(DirectionMask side)
{
  return static_cast<int>(side);
}

std::vector<deskflow::ScreenRect> unevenScreens()
{
  return {
      {-1080, -397, 1080, 1920},
      {0, 0, 1920, 1080},
      {1920, -471, 1080, 1920},
  };
}

deskflow::ScreenRect unevenBounds()
{
  return {-1080, -471, 4080, 1994};
}

} // namespace

void ScreenEdgesTests::projectToVisibleBottomEdge_centerBottomWithLowerSideScreens_projectsToVirtualBottom()
{
  const auto screens = unevenScreens();
  const auto bounds = unevenBounds();
  int32_t x = 960;
  int32_t y = 1079;

  const bool projected = deskflow::projectToVisibleBottomEdge(screens, bottomSide(), 1, bounds, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 960);
  QCOMPARE(y, 1522);
}

void ScreenEdgesTests::projectToVisibleBottomEdge_centerBottomWithinEdgeBand_projectsToVirtualBottom()
{
  const auto screens = unevenScreens();
  const auto bounds = unevenBounds();
  int32_t x = 960;
  int32_t y = 1074;

  const bool projected = deskflow::projectToVisibleBottomEdge(screens, bottomSide(), 8, bounds, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 960);
  QCOMPARE(y, 1522);
}

void ScreenEdgesTests::projectToVisibleBottomEdge_internalScreenBoundary_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {0, 0, 100, 100},
      {0, 100, 100, 100},
  };
  int32_t x = 50;
  int32_t y = 99;

  const bool projected = deskflow::projectToVisibleBottomEdge(screens, bottomSide(), 8, {0, 0, 100, 200}, x, y);

  QVERIFY(!projected);
  QCOMPARE(x, 50);
  QCOMPARE(y, 99);
}

void ScreenEdgesTests::projectToVisibleBottomEdge_inactiveBottomSide_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
  };
  int32_t x = 50;
  int32_t y = 99;

  const bool projected = deskflow::projectToVisibleBottomEdge(screens, 0, 8, {-100, 0, 200, 200}, x, y);

  QVERIFY(!projected);
  QCOMPARE(x, 50);
  QCOMPARE(y, 99);
}

void ScreenEdgesTests::projectToVisibleBottomEdge_rectangularDesktopBottom_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {0, 0, 100, 100},
      {100, 0, 100, 100},
  };
  int32_t x = 50;
  int32_t y = 99;

  const bool projected = deskflow::projectToVisibleBottomEdge(screens, bottomSide(), 8, {0, 0, 200, 100}, x, y);

  QVERIFY(!projected);
  QCOMPARE(x, 50);
  QCOMPARE(y, 99);
}

void ScreenEdgesTests::projectToVisibleEdge_centerRightWithFartherTopAndBottomScreens_projectsToVirtualRight()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {0, -100, 150, 100},
      {0, 0, 100, 100},
      {0, 100, 150, 100},
  };
  int32_t x = 99;
  int32_t y = 50;

  const bool projected =
      deskflow::projectToVisibleEdge(screens, side(DirectionMask::RightMask), 8, {0, -100, 150, 300}, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 149);
  QCOMPARE(y, 50);
}

void ScreenEdgesTests::projectToVisibleEdge_outerRightOverlappingInnerHoleWall_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = 199;
  int32_t y = 150;

  const bool projected =
      deskflow::projectToVisibleEdge(screens, side(DirectionMask::RightMask), 8, {-100, 0, 300, 200}, x, y);

  QVERIFY(!projected);
  QCOMPARE(x, 199);
  QCOMPARE(y, 150);
}

void ScreenEdgesTests::projectToVisibleEdge_outerRightWithInnerHoleWallElsewhere_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = 199;
  int32_t y = 50;

  const bool projected =
      deskflow::projectToVisibleEdge(screens, side(DirectionMask::RightMask), 8, {-100, 0, 300, 200}, x, y);

  QVERIFY(!projected);
  QCOMPARE(x, 199);
  QCOMPARE(y, 50);
}

void ScreenEdgesTests::isBlockedByInternalVisibleEdge_outerRightWithInnerHoleWall_returnsTrue()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };

  QVERIFY(
      deskflow::isBlockedByInternalVisibleEdge(screens, side(DirectionMask::RightMask), 8, {-100, 0, 300, 200}, 199, 50)
  );
}

void ScreenEdgesTests::isBlockedByInternalVisibleEdge_innerHoleWall_returnsFalse()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };

  QVERIFY(!deskflow::isBlockedByInternalVisibleEdge(
      screens, side(DirectionMask::RightMask), 8, {-100, 0, 300, 200}, -1, 150
  ));
}

void ScreenEdgesTests::projectToVisibleEdgeCrossing_fastBottomCrossing_projectsToVirtualBottom()
{
  const auto screens = unevenScreens();
  const auto bounds = unevenBounds();
  int32_t x = 960;
  int32_t y = 1300;

  const bool projected =
      deskflow::projectToVisibleEdgeCrossing(screens, bottomSide(), bounds, 960, 1000, 960, 1300, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 960);
  QCOMPARE(y, 1522);
}

void ScreenEdgesTests::projectToVisibleEdgeCrossing_fastInnerRightCrossing_projectsToVirtualRight()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = 50;
  int32_t y = 150;

  const bool projected = deskflow::projectToVisibleEdgeCrossing(
      screens, side(DirectionMask::RightMask), {-100, 0, 300, 200}, -50, 150, 50, 150, x, y
  );

  QVERIFY(projected);
  QCOMPARE(x, 199);
  QCOMPARE(y, 150);
}

void ScreenEdgesTests::projectToVisibleEdgeCrossing_innerRightWallSnapToBottomCorner_keepsRightWall()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = -1;
  int32_t y = 199;

  const bool projected = deskflow::projectToVisibleEdgeCrossing(
      screens, side(DirectionMask::RightMask) | side(DirectionMask::BottomMask), {-100, 0, 300, 200}, -1, 150, -1, 199,
      x, y
  );

  QVERIFY(projected);
  QCOMPARE(x, 199);
  QCOMPARE(y, 150);
}

void ScreenEdgesTests::projectToVisibleEdgeCrossing_innerRightWallMovedInward_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = -20;
  int32_t y = 150;

  const bool projected = deskflow::projectToVisibleEdgeCrossing(
      screens, side(DirectionMask::RightMask), {-100, 0, 300, 200}, -1, 150, -20, 150, x, y
  );

  QVERIFY(!projected);
  QCOMPARE(x, -20);
  QCOMPARE(y, 150);
}

void ScreenEdgesTests::projectToVisibleEdgeCrossing_fastOuterRightCrossingWithInnerWall_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = 250;
  int32_t y = 50;

  const bool projected = deskflow::projectToVisibleEdgeCrossing(
      screens, side(DirectionMask::RightMask), {-100, 0, 300, 200}, 150, 50, 250, 50, x, y
  );

  QVERIFY(!projected);
  QCOMPARE(x, 250);
  QCOMPARE(y, 50);
}

void ScreenEdgesTests::projectToBoundsEdgeCrossing_fastLeftOvershoot_clampsToCrossedBoundary()
{
  Direction dir = Direction::NoDirection;
  int32_t x = -250;
  int32_t y = 120;

  const bool projected = deskflow::projectToBoundsEdgeCrossing({0, 0, 300, 200}, 20, 100, -250, 120, dir, x, y);

  QVERIFY(projected);
  QCOMPARE(dir, Direction::Left);
  QCOMPARE(x, -1);
  QCOMPARE(y, 101);
}

void ScreenEdgesTests::projectToBoundsEdgeCrossing_fastDiagonalOvershoot_usesFirstCrossedBoundary()
{
  Direction dir = Direction::NoDirection;
  int32_t x = 350;
  int32_t y = 250;

  const bool projected = deskflow::projectToBoundsEdgeCrossing({0, 0, 300, 200}, 250, 100, 350, 250, dir, x, y);

  QVERIFY(projected);
  QCOMPARE(dir, Direction::Right);
  QCOMPARE(x, 300);
  QCOMPARE(y, 174);
}

void ScreenEdgesTests::projectToVisibleEdge_centerLeftWithFartherTopAndBottomScreens_projectsToVirtualLeft()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-50, -100, 150, 100},
      {0, 0, 100, 100},
      {-50, 100, 150, 100},
  };
  int32_t x = 0;
  int32_t y = 50;

  const bool projected =
      deskflow::projectToVisibleEdge(screens, side(DirectionMask::LeftMask), 8, {-50, -100, 150, 300}, x, y);

  QVERIFY(projected);
  QCOMPARE(x, -50);
  QCOMPARE(y, 50);
}

void ScreenEdgesTests::projectToVisibleEdge_centerTopWithHigherSideScreens_projectsToVirtualTop()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-50, -50, 50, 150},
      {0, 0, 100, 100},
      {100, -50, 50, 150},
  };
  int32_t x = 50;
  int32_t y = 0;

  const bool projected =
      deskflow::projectToVisibleEdge(screens, side(DirectionMask::TopMask), 8, {-50, -50, 200, 150}, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 50);
  QCOMPARE(y, -50);
}

void ScreenEdgesTests::projectFromVisibleBottomEdge_virtualBottomHole_projectsToScreenBottom()
{
  const auto screens = unevenScreens();
  int32_t x = 960;
  int32_t y = 1521;

  const bool projected = deskflow::projectFromVisibleBottomEdge(screens, 8, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 960);
  QCOMPARE(y, 1071);
}

void ScreenEdgesTests::projectFromVisibleEdge_virtualRightHole_projectsToScreenRight()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {0, -100, 150, 100},
      {0, 0, 100, 100},
      {0, 100, 150, 100},
  };
  int32_t x = 149;
  int32_t y = 50;

  const bool projected = deskflow::projectFromVisibleEdge(screens, 8, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 91);
  QCOMPARE(y, 50);
}

void ScreenEdgesTests::projectFromVisibleEdge_virtualBottomHoleAtInnerCorner_prefersScreenBottom()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = 0;
  int32_t y = 199;

  const bool projected = deskflow::projectFromVisibleEdge(screens, 8, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 0);
  QCOMPARE(y, 91);
}

void ScreenEdgesTests::projectFromVisibleEdge_virtualRightHole_prefersInternalScreenRight()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = 200;
  int32_t y = 150;

  const bool projected = deskflow::projectFromVisibleEdge(screens, 8, x, y);

  QVERIFY(projected);
  QCOMPARE(x, -9);
  QCOMPARE(y, 150);
}

void ScreenEdgesTests::projectFromVisibleEdge_insideVirtualRightEdge_prefersInternalScreenRight()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = 199;
  int32_t y = 150;

  const bool projected = deskflow::projectFromVisibleEdge(screens, 8, x, y);

  QVERIFY(projected);
  QCOMPARE(x, -9);
  QCOMPARE(y, 150);
}

void ScreenEdgesTests::projectFromVisibleEdge_virtualLeftHole_prefersInternalScreenLeft()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {-100, 0, 100, 200},
      {0, 0, 100, 100},
      {100, 0, 100, 200},
  };
  int32_t x = -100;
  int32_t y = 150;

  const bool projected = deskflow::projectFromVisibleEdge(screens, 8, x, y);

  QVERIFY(projected);
  QCOMPARE(x, 108);
  QCOMPARE(y, 150);
}

void ScreenEdgesTests::projectFromVisibleBottomEdge_positionInsideScreen_preservesPosition()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {0, 0, 1920, 1080},
  };
  int32_t x = 960;
  int32_t y = 1078;

  const bool projected = deskflow::projectFromVisibleBottomEdge(screens, 1, x, y);

  QVERIFY(!projected);
  QCOMPARE(x, 960);
  QCOMPARE(y, 1078);
}

void ScreenEdgesTests::visibleBottomEdgeInterval_centerBottomWithLowerSideScreens_returnsCenterInterval()
{
  const auto screens = unevenScreens();
  const auto bounds = unevenBounds();
  const auto interval = deskflow::visibleBottomEdgeInterval(screens, bounds, screens[1]);

  QVERIFY(interval);
  QCOMPARE(interval->start, 26.470588235294116);
  QCOMPARE(interval->end, 73.529411764705884);
}

void ScreenEdgesTests::visibleEdgeInterval_centerRightWithFartherTopAndBottomScreens_returnsCenterInterval()
{
  const std::vector<deskflow::ScreenRect> screens = {
      {0, -100, 150, 100},
      {0, 0, 100, 100},
      {0, 100, 150, 100},
  };
  const auto interval = deskflow::visibleEdgeInterval(screens, {0, -100, 150, 300}, screens[1], Direction::Right);

  QVERIFY(interval);
  QCOMPARE(interval->start, 33.333333333333336);
  QCOMPARE(interval->end, 66.666666666666671);
}

QTEST_MAIN(ScreenEdgesTests)
