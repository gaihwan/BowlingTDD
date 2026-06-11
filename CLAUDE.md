# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

Visual Studio 2022 + GMock(gtest) 환경에서 볼링 점수 계산기를 TDD로 구현하는 Kata 실습 프로젝트.  
구현 목표: [`doc/Bowling-KATA.md`](doc/Bowling-KATA.md) 참고.

- `Game::roll(int pins)` — 투구마다 호출
- `Game::score()` — 최종 총점 반환 (스트라이크·스페어 보너스 포함)

## 빌드 및 실행

```powershell
# 빌드 (x64 Release)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" BowlingTDD.sln /p:Configuration=Release /p:Platform=x64

# 테스트 실행
.\x64\Release\BowlingTDD.exe
```

출력 예시:
```
[==========] Running N tests from N test suites.
[  PASSED  ] N tests.
```

## 프로젝트 구조 핵심

- **테스트 진입점**: `BowlingTDD/main.cpp` — `::testing::InitGoogleMock` + `RUN_ALL_TESTS()`
- **gtest/gmock**: NuGet 패키지 `gmock 1.11.0` (`packages/gmock.1.11.0/`) — `#include <gtest/gtest.h>` / `#include <gmock/gmock.h>` 로 바로 사용
- **소스 등록**: 새 `.cpp` 파일 추가 시 `BowlingTDD/BowlingTDD.vcxproj` 의 `<ItemGroup>` 에 `<ClCompile Include="파일명.cpp" />` 추가 필요
- **단일 구성**: Debug/Release 모두 단일 실행 파일 빌드. 별도 Debug 분기 없음

## TDD 진행 방식

> **스킬:** 기능 개발 또는 버그 수정 전 반드시 [`.claude/skills/test-driven-development/SKILL.md`](.claude/skills/test-driven-development/SKILL.md) (`test-driven-development`) 를 참고한다.

Red → Green → Refactor 사이클을 커밋 단위로 구분한다.

```
test(game): Add failing test for gutter game   ← Red
feat(game): Implement gutter game score        ← Green
refactor(game): Simplify roll loop             ← Refactor
```

권장 테스트 순서 (Kata 난이도 순):

| 단계 | 시나리오 | 기댓값 |
|------|----------|--------|
| 1 | 거터 게임 — 20회 연속 0 | 0 |
| 2 | 올 원 — 20회 연속 1 | 20 |
| 3 | 스페어 한 번 — 5,5,3,0…0 | 16 |
| 4 | 스트라이크 한 번 — 10,3,4,0…0 | 24 |
| 5 | 퍼펙트 게임 — 12회 연속 10 | 300 |

## Git Commit 규칙

커밋 전 **[`doc/COMMIT_CONVENTION.md`](doc/COMMIT_CONVENTION.md)** 를 반드시 확인한다.  
형식: `<type>(<scope>): <subject>` — type은 `test` / `feat` / `refactor` / `fix` / `docs` / `chore`.
