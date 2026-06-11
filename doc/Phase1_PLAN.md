# Phase 1 — 거터 게임 (Gutter Game) 세부 실행 계획

> **담당 에이전트**: red-agent → build-git-agent → green-agent → build-git-agent → reviewer-agent → (refactor-agent) → build-git-agent  
> **목표**: `roll(0)` × 20 → `score() == 0`  
> **전제 조건**: 프로젝트 초기 상태 (Game.h, Game.cpp, test_bowling.cpp 미존재)

---

## 사전 작업

`BowlingTDD/main.cpp` 에 더미 테스트(`SampleTest::AssertionFails`)가 있다.  
이를 **RED 단계에서 함께 제거**한다. 이 테스트는 항상 실패하므로 GREEN 검증이 불가능하다.

---

## STEP 1 — RED (red-agent)

### 에이전트: `red-agent`

### 전달 내용

```
시나리오: Phase 1 — 거터 게임
목표: roll(0) × 20 → score() == 0

[사전 작업] main.cpp 정리
BowlingTDD/main.cpp 에서 SampleTest::AssertionFails 테스트 블록을 제거하고
InitGoogleMock + RUN_ALL_TESTS() 진입점만 남긴다.

[파일 생성 1] BowlingTDD/test_bowling.cpp (신규)
  - 헬퍼 함수 namespace 블록 포함:
      namespace {
          void rollMany(Game& game, int n, int pins) {
              for (int i = 0; i < n; ++i) game.roll(pins);
          }
          void rollSpare(Game& game) { game.roll(5); game.roll(5); }
          void rollStrike(Game& game) { game.roll(10); }
      }
  - 테스트 케이스:
      TEST(BowlingGameTest, GutterGameScoresZero) {
          Game game;
          rollMany(game, 20, 0);
          ASSERT_EQ(0, game.score());
      }

[파일 생성 2] BowlingTDD/Game.h (신규 — 스텁)
  #pragma once
  class Game {
  public:
      void roll(int pins);
      int  score();
  };

[파일 생성 3] BowlingTDD/Game.cpp (신규 — 빈 구현)
  #include "Game.h"
  void Game::roll(int /*pins*/) {}
  int  Game::score() { return -1; }  // 의도적으로 잘못된 값

[vcxproj 수정] BowlingTDD/BowlingTDD.vcxproj
  <ItemGroup> 에 아래 두 줄 추가:
  <ClCompile Include="test_bowling.cpp" />
  <ClCompile Include="Game.cpp" />

참조: doc/Bowling-KATA.md, .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `test_bowling.cpp`, `Game.h`, `Game.cpp` 생성 완료
- `BowlingTDD.vcxproj` 에 두 파일 등록
- `main.cpp` 더미 테스트 제거 완료

---

## STEP 2 — RED 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: RED 검증 — Phase 1 (거터 게임)
작업 디렉토리: D:\02_Study\02_ClaudeCode\reviewer\TAKA\BowlingTDD

빌드 명령:
  $msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
  & $msbuild BowlingTDD.sln /p:Configuration=Release /p:Platform=x64 /v:minimal

테스트 실행:
  .\x64\Release\BowlingTDD.exe

예상 결과:
  [  FAILED  ] BowlingGameTest.GutterGameScoresZero
  Expected: 0
  Actual:   -1
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일 오류 | ❌ | red-agent 재호출 (최대 3회) |
| GutterGameScoresZero PASS | ❌ RED 무효 | red-agent 에게 테스트 재작성 요청 |
| GutterGameScoresZero FAIL (Expected 0, Actual -1) | ✅ | STEP 3 진행 |

---

## STEP 3 — GREEN (green-agent)

### 에이전트: `green-agent`

### 전달 내용

```
시나리오: Phase 1 — 거터 게임
실패 테스트: BowlingGameTest::GutterGameScoresZero
실패 메시지: Expected 0, Actual -1

수정 파일: BowlingTDD/Game.cpp
수정 내용: score() 가 0을 반환하도록 최소 구현
  int Game::score() { return 0; }

Game.h 수정 불필요.
기존 통과 테스트: 없음 (첫 번째 시나리오)

참조: .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `Game.cpp` 의 `score()` 가 `return 0;` 으로 변경됨
- `Game.h` 변경 없음
- 불필요한 로직 추가 없음

---

## STEP 4 — GREEN 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: GREEN 검증 — Phase 1 (거터 게임)
예상 결과: 전체 1건 통과
  [  OK  ] BowlingGameTest.GutterGameScoresZero
  [  PASSED  ] 1 test.
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일/링커 오류 | ❌ | green-agent 재호출 (최대 3회) |
| 테스트 실패 | ❌ | green-agent 재호출 |
| 전체 1건 통과 | ✅ | STEP 5 진행 |

---

## STEP 5 — REVIEW (reviewer-agent)

### 에이전트: `reviewer-agent`

### 전달 내용

```
시나리오: Phase 1 GREEN 완료
리뷰 대상:
  - BowlingTDD/Game.h  (스텁 선언)
  - BowlingTDD/Game.cpp (score() { return 0; })

GREEN 구현 요약:
  - roll(): 빈 구현
  - score(): return 0 (하드코딩)

현재 통과 테스트: BowlingGameTest::GutterGameScoresZero (1건)
```

### 예상 판정

**리팩터링 불필요** — `score() { return 0; }` 은 Phase 1 수준의 의도적 최소 구현이다.

### 판정에 따른 분기

| 판정 | 처리 |
|------|------|
| 리팩터링 불필요 | STEP 6 스킵 → STEP 7 (커밋) 으로 바로 이동 |
| 리팩터링 필요 | STEP 6 (refactor-agent) 호출 |

---

## STEP 6 — REFACTOR (refactor-agent) ← 조건부

### 에이전트: `refactor-agent`

> Phase 1은 대부분 실행되지 않는 단계다. reviewer-agent 판정이 "필요"일 때만 실행.

### 전달 내용 (필요 시)

```
시나리오: Phase 1 REFACTOR
reviewer-agent 지적 사항: <reviewer 출력 내용 전달>

수정 범위: reviewer가 지적한 항목만 수정
모든 테스트가 통과하는 상태를 유지할 것
```

### REFACTOR 검증 (build-git-agent)

```
단계: REFACTOR 검증 — Phase 1
예상 결과: 전체 1건 통과
REFACTOR 전후 동일한 테스트 결과 확인
```

---

## STEP 7 — 커밋 (build-git-agent)

> 사용자 검토 요청 후 승인 시 실행

### 사용자 검토 요청 형식

```
[사용자 검토 요청 — Phase 1: 거터 게임]
─────────────────────────────────────
빌드:   ✅ 성공
테스트: ✅ 전체 1건 통과
─────────────────────────────────────
변경된 파일:
  [RED]      main.cpp        — 더미 테스트 제거
  [RED]      test_bowling.cpp — GutterGameScoresZero 추가
  [RED]      Game.h          — 스텁 선언
  [RED]      Game.cpp        — 빈 구현 (score returns -1)
  [GREEN]    Game.cpp        — score() { return 0; }
  [REVIEW]   변경 없음
  [REFACTOR] 변경 없음 (스킵)

예정 커밋:
  test(game): Add failing test for gutter game
  feat(game): Implement gutter game score
```

### 커밋 명령

```powershell
# RED 커밋
git add BowlingTDD/main.cpp BowlingTDD/test_bowling.cpp BowlingTDD/Game.h BowlingTDD/Game.cpp BowlingTDD/BowlingTDD.vcxproj
git commit -m "$(cat <<'EOF'
test(game): Add failing test for gutter game

Create Game stub and test; ASSERT_EQ(0, game.score()) fails as
expected with score() returning -1 (Red phase).

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# GREEN 커밋
git add BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
feat(game): Implement gutter game score

Return 0 from score() — minimum implementation to pass gutter game test.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# REFACTOR 커밋: 변경 없으면 스킵

git push origin master
```

---

## Phase 1 완료 후 상태

```
BowlingTDD/
  main.cpp           ← 더미 테스트 제거됨
  test_bowling.cpp   ← GutterGameScoresZero 포함
  Game.h             ← void roll(int), int score() 선언
  Game.cpp           ← roll() 빈 구현, score() { return 0; }
  BowlingTDD.vcxproj ← test_bowling.cpp, Game.cpp 등록됨

통과 테스트:
  ✅ BowlingGameTest::GutterGameScoresZero

다음 단계: Phase2_PLAN.md (올 원)
```
