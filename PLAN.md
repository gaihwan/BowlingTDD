# Bowling Kata TDD 실행 계획

## 문서 구조

이 계획은 **마스터 계획(PLAN.md)** 과 **Phase별 세부 계획** 으로 구성된다.

```
PLAN.md               ← 전체 개요, 에이전트 구성, 사이클 흐름 (이 파일)
doc/Phase1_PLAN.md    ← 거터 게임 단계별 실행 계획 (각 에이전트 전달 내용 포함)
doc/Phase2_PLAN.md    ← 올 원
doc/Phase3_PLAN.md    ← 스페어 한 번
doc/Phase4_PLAN.md    ← 스트라이크 한 번
doc/Phase5_PLAN.md    ← 퍼펙트 게임
```

**tdd-orchestrator** 는 각 시나리오 진입 시 해당 Phase PLAN.md 를 참조하여  
각 에이전트에게 전달할 내용을 그대로 복사해 사용한다.

---

## TDD 사이클 (업데이트됨)

```
RED → [빌드검증] → GREEN → [빌드검증] → REVIEW → (필요 시) REFACTOR → [빌드검증] → 커밋
                                                ↑
                                         reviewer-agent 가
                                         GREEN 코드를 리뷰하고
                                         REFACTOR 필요 여부 판정
```

---

## 전제 조건 및 현재 상태

### 현재 프로젝트 상태

- `BowlingTDD/main.cpp`: 더미 테스트(`SampleTest::AssertionFails`) 포함된 진입점
- `BowlingTDD/BowlingTDD.vcxproj`: `main.cpp` 만 `<ClCompile>` 등록된 상태
- `Game.h`, `Game.cpp`, `test_bowling.cpp`: **미존재** (새로 생성 필요)
- gtest/gmock: NuGet 패키지 `gmock 1.11.0` 로 제공 (`packages/gmock.1.11.0/`)

### 빌드 명령 (공통)

```powershell
$msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
& $msbuild "D:\02_Study\02_ClaudeCode\reviewer\TAKA\BowlingTDD\BowlingTDD.sln" /p:Configuration=Release /p:Platform=x64 /v:minimal
.\x64\Release\BowlingTDD.exe
```

### 사전 작업 (시나리오 1 RED 전)

`main.cpp` 의 더미 테스트(`SampleTest::AssertionFails`)를 제거한다.  
이 테스트는 항상 실패하므로 GREEN/REFACTOR 검증에서 전체 통과 판정이 불가능하다.  
→ **시나리오 1 RED 에이전트의 첫 번째 지시 사항으로 포함**한다.

---

## 파일 구조 변화 예측

### 시나리오 1 RED 이후 생성되는 파일

```
BowlingTDD/
  main.cpp            ← 더미 테스트 제거, main() 진입점만 유지
  test_bowling.cpp    ← 신규 생성 (테스트 전용)
  Game.h              ← 신규 생성 (선언 스텁)
  Game.cpp            ← 신규 생성 (빈 구현, score() returns -1)
  BowlingTDD.vcxproj  ← ItemGroup에 3개 파일 추가
```

### 이후 시나리오에서 변경되는 파일

```
BowlingTDD/
  test_bowling.cpp  ← 시나리오마다 TEST() 블록 누적 추가
  Game.h            ← 멤버 추가 (rolls_[], current_roll_, private 메서드)
  Game.cpp          ← score()/roll() 구현 누적 발전
```

`main.cpp` 와 `BowlingTDD.vcxproj` 는 시나리오 1 이후 추가 변경 없음.

---

## 에이전트 구성

| 에이전트 | 역할 | 정의 파일 |
|----------|------|-----------|
| `red-agent` | 실패하는 테스트 작성 | `.claude/agents/red_agent.md` |
| `green-agent` | 최소 프로덕션 코드 구현 | `.claude/agents/green_agent.md` |
| `reviewer-agent` | GREEN 코드 리뷰 → REFACTOR 필요 여부 판정 | `.claude/agents/reviewer_agent.md` |
| `refactor-agent` | REVIEWER 지적 사항 기반 코드 품질 개선 | `.claude/agents/refactor_agent.md` |
| `build-git-agent` | 빌드·검증·Git 커밋·푸시 | `.claude/agents/build_agent.md` |

---

## 한 사이클 흐름 (업데이트됨)

```
RED → [빌드검증] → GREEN → [빌드검증] → REVIEW → (필요 시) REFACTOR → [빌드검증] → 사용자승인 → 커밋/푸시
```

### REVIEWER 단계 분기

```
REVIEW 판정: "리팩터링 필요"  →  REFACTOR 에이전트 호출 → 빌드검증 → 커밋
REVIEW 판정: "리팩터링 불필요" →  REFACTOR 스킵         →           → 커밋
```

---

## 공통 에이전트 호출 프로토콜

### 재시도 상한

- `red-agent`, `green-agent`, `refactor-agent` 재시도 한도: **3회**
- `reviewer-agent` 재시도 없음 (판정 결과만 반환)
- 3회 연속 동일 오류 시 tdd-orchestrator가 사용자에게 보고하고 대기

### 성공/실패 판정 기준 총람

| 단계 | 성공 조건 | 실패 처리 |
|------|-----------|-----------|
| RED 검증 | 컴파일 성공 + 신규 테스트 FAIL (ASSERT_EQ 값 불일치) | 컴파일 오류 → red-agent 수정 재요청 |
| RED 검증 | 기존 테스트 모두 PASS 유지 | 기존 테스트 깨짐 → red-agent 수정 재요청 |
| GREEN 검증 | 전체 N건 PASS | 임의 실패 → green-agent 수정 재요청 |
| REVIEW | 리뷰 판정 반환 | 해당 없음 (판정만 수행) |
| REFACTOR 검증 | 전체 N건 PASS (변경 전후 동일) | 회귀 발생 → refactor-agent 복구 재요청 |
| 커밋 | git status 가 clean, 빌드 산출물 미포함 | build-git-agent 스테이징 재확인 |

---

## 시나리오별 상세 절차

---

### 시나리오 1 — 거터 게임 (Gutter Game)

**목표**: `roll(0)` × 20 → `score() == 0`

#### RED 단계

**red-agent 에게 전달할 내용**

```
시나리오: 1번 — 거터 게임
작업:
  1. BowlingTDD/main.cpp 에서 SampleTest::AssertionFails 테스트를 제거하고
     main() 진입점 코드만 남긴다.

  2. BowlingTDD/test_bowling.cpp 를 새로 생성하고 아래 테스트를 작성한다:
     TEST(BowlingGameTest, GutterGameScoresZero) {
         Game game;
         for (int i = 0; i < 20; ++i)
             game.roll(0);
         ASSERT_EQ(0, game.score());
     }

  3. BowlingTDD/Game.h 를 생성한다 (스텁):
     class Game {
     public:
         void roll(int pins);
         int  score();
     };

  4. BowlingTDD/Game.cpp 를 생성한다 (빈 구현):
     void Game::roll(int) {}
     int  Game::score() { return -1; }

  5. BowlingTDD.vcxproj 의 <ItemGroup> 에 아래 항목을 추가한다:
     <ClCompile Include="test_bowling.cpp" />
     <ClCompile Include="Game.cpp" />

헬퍼 함수 namespace 블록도 test_bowling.cpp 상단에 추가한다 (rollMany 등).
참조: doc/Bowling-KATA.md, .claude/skills/test-driven-development/SKILL.md
```

**RED 검증 (build-git-agent 에게 전달할 내용)**

```
단계: RED 검증 — 시나리오 1 (거터 게임)
예상 결과: BowlingGameTest::GutterGameScoresZero 가 실패해야 한다.
예상 실패 메시지:
  ASSERT_EQ(0, game.score()) — score()가 -1을 반환하므로
  Expected: 0
  Actual:   -1
컴파일 오류 시: red-agent 에게 수정 재요청
테스트 통과 시: red-agent 에게 테스트 재작성 요청
```

**성공 판정**: `[ FAILED ] BowlingGameTest.GutterGameScoresZero` 출력, Expected 0 / Actual -1 확인

#### GREEN 단계

**green-agent 에게 전달할 내용**

```
시나리오: 1번 — 거터 게임
실패 테스트: BowlingGameTest::GutterGameScoresZero
실패 메시지: ASSERT_EQ(0, game.score()), Actual: -1

BowlingTDD/Game.cpp 의 score() 를 수정하여 테스트를 통과시켜라.
최소 구현: score() { return 0; }
Game.h 변경 불필요.

기존 테스트: 없음 (이번이 첫 번째)
```

**GREEN 검증 (build-git-agent 에게 전달할 내용)**

```
단계: GREEN 검증 — 시나리오 1 (거터 게임)
예상 결과: 전체 테스트 통과 (현재 1건)
[ OK ] BowlingGameTest.GutterGameScoresZero
```

**성공 판정**: `[  PASSED  ] 1 test.` 출력

#### REFACTOR 단계

**refactor-agent 에게 전달할 내용**

```
시나리오 1 GREEN 완료 후 리팩터링 검토.
현재 파일:
  - BowlingTDD/Game.h  (스텁 선언)
  - BowlingTDD/Game.cpp (score() { return 0; })
  - BowlingTDD/test_bowling.cpp

GREEN 구현: score()가 하드코딩 0 반환.
구조가 매우 단순하므로 "변경 불필요" 판정 가능성 높음.
```

**예상 결과**: 변경 불필요 (하드코딩 0은 이 단계에서 의도적)

#### 커밋 전략 (사용자 승인 후 build-git-agent 실행)

```
커밋 1 (RED):
  git add BowlingTDD/main.cpp BowlingTDD/test_bowling.cpp BowlingTDD/Game.h BowlingTDD/Game.cpp BowlingTDD/BowlingTDD.vcxproj
  메시지: test(game): Add failing test for gutter game

  ASSERT_EQ(0, game.score()) fails as expected (Red phase).

커밋 2 (GREEN):
  git add BowlingTDD/Game.cpp
  메시지: feat(game): Implement gutter game score

  Return 0 from score() — minimum to pass gutter game test.

커밋 3 (REFACTOR): 스킵 (변경 없음)

git push origin master
```

---

### 시나리오 2 — 올 원 (All Ones)

**목표**: `roll(1)` × 20 → `score() == 20`

#### RED 단계

**red-agent 에게 전달할 내용**

```
시나리오: 2번 — 올 원
기존 테스트 파일: BowlingTDD/test_bowling.cpp (GutterGameScoresZero 포함)
기존 프로덕션 파일: BowlingTDD/Game.h, BowlingTDD/Game.cpp

test_bowling.cpp 에 아래 테스트를 추가한다 (기존 테스트는 건드리지 않는다):

TEST(BowlingGameTest, AllOnesScoresTwenty) {
    Game game;
    for (int i = 0; i < 20; ++i)
        game.roll(1);
    ASSERT_EQ(20, game.score());
}

Game.h / Game.cpp 수정 금지. 현재 score()는 0을 반환하므로
ASSERT_EQ(20, game.score()) 는 올바른 이유로 실패한다.
```

**RED 검증**

```
단계: RED 검증 — 시나리오 2 (올 원)
예상 실패: BowlingGameTest::AllOnesScoresTwenty
예상 메시지: Expected 20, Actual 0
기존 테스트 GutterGameScoresZero 는 여전히 통과해야 한다 ([ OK ] 확인 필수)
```

**성공 판정**: GutterGameScoresZero PASS + AllOnesScoresTwenty FAIL (Expected 20, Actual 0)

#### GREEN 단계

**green-agent 에게 전달할 내용**

```
시나리오: 2번 — 올 원
실패 테스트: BowlingGameTest::AllOnesScoresTwenty
실패 메시지: Expected 20, Actual 0
기존 통과 테스트: BowlingGameTest::GutterGameScoresZero (깨지지 않아야 함)

Game.h 에 멤버 추가 (rolls_ 배열, current_roll_):
  int rolls_[21]{};
  int current_roll_ = 0;

Game.cpp 구현:
  roll(): rolls_[current_roll_++] = pins;
  score(): rolls_ 배열 단순 합산 (0~19 인덱스)

두 테스트 모두 통과해야 한다.
```

**GREEN 검증**

```
단계: GREEN 검증 — 시나리오 2
예상 결과: 전체 2건 통과
[ OK ] BowlingGameTest.GutterGameScoresZero
[ OK ] BowlingGameTest.AllOnesScoresTwenty
```

#### REFACTOR 단계

**refactor-agent 에게 전달할 내용**

```
시나리오 2 GREEN 완료.
현재 Game.cpp score() 내 루프 점검 사항:
  - score() 루프 변수명이 의미를 드러내는가 (i → roll_index 등)
  - 중복 없음 확인
  - 20이라는 매직 넘버가 있다면 상수로 추출 검토
```

**예상 결과**: 루프 인덱스 이름 정리 정도 가능 (소규모 변경 또는 변경 불필요)

#### 커밋 전략

```
커밋 1 (RED):
  git add BowlingTDD/test_bowling.cpp
  메시지: test(game): Add failing test for all-ones game

  ASSERT_EQ(20, game.score()) fails because score() returns 0.

커밋 2 (GREEN):
  git add BowlingTDD/Game.h BowlingTDD/Game.cpp
  메시지: feat(game): Accumulate rolls and sum for score

  Introduce rolls_ array and current_roll_ to enable
  per-roll tracking and total sum calculation.

커밋 3 (REFACTOR): 변경이 있을 때만
  git add BowlingTDD/Game.cpp
  메시지: refactor(game): Rename loop variable to roll_index

git push origin master
```

---

### 시나리오 3 — 스페어 한 번 (One Spare)

**목표**: `roll(5), roll(5), roll(3)`, 나머지 0 → `score() == 16`

#### RED 단계

**red-agent 에게 전달할 내용**

```
시나리오: 3번 — 스페어 한 번
기존 테스트 파일: BowlingTDD/test_bowling.cpp
기존 통과 테스트: GutterGameScoresZero, AllOnesScoresTwenty

test_bowling.cpp 에 아래 테스트를 추가한다:

TEST(BowlingGameTest, OneSpareAddsNextRollAsBonus) {
    Game game;
    rollSpare(game);           // 5, 5 (헬퍼 사용)
    game.roll(3);
    rollMany(game, 17, 0);     // 나머지 0
    ASSERT_EQ(16, game.score());
}

rollSpare 헬퍼가 test_bowling.cpp 에 없다면 namespace 블록에 추가한다:
  void rollSpare(Game& game) { game.roll(5); game.roll(5); }

현재 score()는 단순 합산이므로 16이 아닌 13을 반환한다.
(5+5+3=13, 보너스 3이 누락)
```

**RED 검증**

```
단계: RED 검증 — 시나리오 3 (스페어)
기존 2건 통과 확인 필수
예상 실패: OneSpareAddsNextRollAsBonus
예상 메시지: Expected 16, Actual 13
```

**성공 판정**: 2건 PASS + 1건 FAIL (Expected 16, Actual 13)

#### GREEN 단계

**green-agent 에게 전달할 내용**

```
시나리오: 3번 — 스페어
실패 테스트: BowlingGameTest::OneSpareAddsNextRollAsBonus
실패 메시지: Expected 16, Actual 13
기존 통과 테스트: GutterGameScoresZero, AllOnesScoresTwenty

Game.cpp 의 score() 를 프레임 단위 루프로 재작성한다.
스페어(두 투구 합 == 10) 감지 시 다음 투구를 보너스로 추가.

Game.h 에 private 메서드 isSpare(int roll_index) 선언 추가.

배열 크기: rolls_[21] 유지 (현재와 동일).
roll_index 로 프레임을 추적한다.
```

**GREEN 검증**

```
단계: GREEN 검증 — 시나리오 3
예상 결과: 전체 3건 통과
```

#### REFACTOR 단계

**refactor-agent 에게 전달할 내용**

```
시나리오 3 GREEN 완료.
점검 사항:
  - isSpare() 조건이 score() 내에 인라인으로 남아있다면 메서드로 추출
  - 프레임 루프 변수명 (frame, roll_index) 일관성 확인
  - 매직 넘버 10 (핀 수) 처리 여부 확인
```

**예상 결과**: isSpare() 메서드 추출 (코드에 따라 다름)

#### 커밋 전략

```
커밋 1 (RED):
  git add BowlingTDD/test_bowling.cpp
  메시지: test(game): Add failing test for spare bonus

  ASSERT_EQ(16, game.score()) fails; current sum misses spare bonus.

커밋 2 (GREEN):
  git add BowlingTDD/Game.h BowlingTDD/Game.cpp
  메시지: feat(game): Add spare bonus to frame-based score

  Rewrite score() with frame loop; add 10+next roll when spare detected.

커밋 3 (REFACTOR): 변경이 있을 때만
  git add BowlingTDD/Game.h BowlingTDD/Game.cpp
  메시지: refactor(game): Extract isSpare helper method

git push origin master
```

---

### 시나리오 4 — 스트라이크 한 번 (One Strike)

**목표**: `roll(10), roll(3), roll(4)`, 나머지 0 → `score() == 24`

#### RED 단계

**red-agent 에게 전달할 내용**

```
시나리오: 4번 — 스트라이크 한 번
기존 통과 테스트: GutterGameScoresZero, AllOnesScoresTwenty,
                 OneSpareAddsNextRollAsBonus

test_bowling.cpp 에 아래 테스트를 추가한다:

TEST(BowlingGameTest, OneStrikeAddsNextTwoRollsAsBonus) {
    Game game;
    rollStrike(game);          // 10 (헬퍼 사용)
    game.roll(3);
    game.roll(4);
    rollMany(game, 16, 0);     // 나머지 0 (10프레임까지)
    ASSERT_EQ(24, game.score());
}

rollStrike 헬퍼가 없다면 추가한다:
  void rollStrike(Game& game) { game.roll(10); }

현재 구현은 스트라이크 프레임에서 1구를 소비하지 않고
2구로 처리하므로 보너스 계산이 틀린다.
예상 실패: Expected 24, Actual (잘못된 값)
```

**RED 검증**

```
단계: RED 검증 — 시나리오 4 (스트라이크)
기존 3건 통과 확인 필수
예상 실패: OneStrikeAddsNextTwoRollsAsBonus
Expected 24, Actual ≠ 24
```

#### GREEN 단계

**green-agent 에게 전달할 내용**

```
시나리오: 4번 — 스트라이크
실패 테스트: BowlingGameTest::OneStrikeAddsNextTwoRollsAsBonus
실패 메시지: Expected 24, Actual <잘못된 값>
기존 통과 테스트 3건 모두 유지해야 함

score() 에 스트라이크 감지 로직 추가:
  - 첫 투구 == 10 이면 스트라이크
  - 보너스: 다음 2회 투구 합산
  - roll_index += 1 (스트라이크는 프레임에서 1구만 소비)
  - 스페어 검사보다 스트라이크 검사가 먼저 와야 함

Game.h 에 isStrike(int roll_index) 선언 추가.
rolls_[21] 배열 크기 유지 (인덱스 0~20, 퍼펙트 게임 12구 수용 가능).
```

**GREEN 검증**

```
단계: GREEN 검증 — 시나리오 4
예상 결과: 전체 4건 통과
```

#### REFACTOR 단계

**refactor-agent 에게 전달할 내용**

```
시나리오 4 GREEN 완료.
점검 사항:
  - isStrike() 메서드 추출 여부 (score() 내 인라인 조건이 있다면 추출)
  - isSpare() 와 isStrike() 의 const/noexcept 적용 여부
  - score() 프레임 루프 구조의 가독성 (if/else if/else 블록 정렬)
  - roll_index 증감 로직의 명확성
```

**예상 결과**: isStrike() 메서드 추출, const 적용

#### 커밋 전략

```
커밋 1 (RED):
  git add BowlingTDD/test_bowling.cpp
  메시지: test(game): Add failing test for strike bonus

  ASSERT_EQ(24, game.score()) fails; strike bonus not implemented.

커밋 2 (GREEN):
  git add BowlingTDD/Game.h BowlingTDD/Game.cpp
  메시지: feat(game): Add strike bonus to score calculation

  Detect first-ball 10, add next two rolls as bonus,
  advance roll_index by 1 for strike frame.

커밋 3 (REFACTOR): 변경이 있을 때만
  git add BowlingTDD/Game.h BowlingTDD/Game.cpp
  메시지: refactor(game): Extract isStrike and apply const noexcept

git push origin master
```

---

### 시나리오 5 — 퍼펙트 게임 (Perfect Game)

**목표**: `roll(10)` × 12 → `score() == 300`

#### RED 단계

**red-agent 에게 전달할 내용**

```
시나리오: 5번 — 퍼펙트 게임
기존 통과 테스트: GutterGameScoresZero, AllOnesScoresTwenty,
                 OneSpareAddsNextRollAsBonus, OneStrikeAddsNextTwoRollsAsBonus

test_bowling.cpp 에 아래 테스트를 추가한다:

TEST(BowlingGameTest, PerfectGameScoresThreeHundred) {
    Game game;
    for (int i = 0; i < 12; ++i)
        game.roll(10);
    ASSERT_EQ(300, game.score());
}

현재 rolls_[21] 배열에 12개를 저장하는 데 문제가 없는지 확인:
  12 < 21 이므로 배열 범위 초과 없음.
  하지만 score() 프레임 루프에서 roll_index 가 배열 경계를 넘을 수 있음.
  예상 실패: Expected 300, Actual ≠ 300
```

> **⚠️ 퍼펙트 게임 RED 즉시 통과 케이스**  
> 시나리오 4 구현이 완벽하다면 이 테스트가 이미 통과할 수 있다.  
> 이 경우 build-git-agent 가 "RED 무효 — 이미 통과" 로 판정한다.  
> → RED 커밋 메시지를 `test(game): Add perfect game test (passes with existing impl)` 로 기록하고  
> → GREEN 단계를 스킵, REFACTOR 단계로 바로 진행한다.

**RED 검증**

```
단계: RED 검증 — 시나리오 5 (퍼펙트 게임)
기존 4건 통과 확인 필수
예상 실패: PerfectGameScoresThreeHundred
Expected 300, Actual ≠ 300
(이미 통과 시: "RED 무효" 판정 후 GREEN 스킵)
```

#### GREEN 단계

**green-agent 에게 전달할 내용**

```
시나리오: 5번 — 퍼펙트 게임
실패 테스트: BowlingGameTest::PerfectGameScoresThreeHundred
(이미 통과하면 이 단계 스킵)

점검 사항:
  - rolls_[21] 배열: 12구 스트라이크 저장 가능 (0~11 사용)
  - 10번 프레임 스트라이크 처리 시 roll_index + 1, +2 가 배열 범위 내인지 확인
  - 배열 범위 초과 없으면 시나리오 4 구현으로 통과 가능

추가 코드가 필요하다면 최소한만 변경한다.
```

#### REFACTOR 단계

**refactor-agent 에게 전달할 내용**

```
시나리오 5 GREEN 완료. 전체 구현 완성 시점의 최종 리팩터링.
점검 사항:
  1. const/noexcept 전면 적용:
     - roll() noexcept
     - score() const noexcept
     - isStrike() const noexcept
     - isSpare() const noexcept

  2. 매직 넘버 상수화:
     - 프레임 수 10 → static constexpr int kTotalFrames = 10;
     - (선택) 핀 수 10 → static constexpr int kMaxPins = 10;

  3. 배열 크기 문서화:
     - rolls_[21] 에 주석: // 10 frames × 2 rolls + 1 bonus = 21 max

  4. 헤더-구현 분리 최종 점검

모든 5건의 테스트가 통과 상태를 유지해야 한다.
```

#### 커밋 전략

```
커밋 1 (RED):
  시나리오 4 구현으로 이미 통과하는 경우:
    git add BowlingTDD/test_bowling.cpp
    메시지: test(game): Add perfect game test (passes with existing impl)

    roll(10) × 12 scores 300; strike logic from scenario 4 handles it.

  실패하는 경우:
    git add BowlingTDD/test_bowling.cpp
    메시지: test(game): Add failing test for perfect game

    ASSERT_EQ(300, game.score()) fails; 12 strikes not fully handled.

커밋 2 (GREEN): 추가 구현이 있을 때만
  git add BowlingTDD/Game.h BowlingTDD/Game.cpp
  메시지: feat(game): Fix perfect game boundary in score loop

커밋 3 (REFACTOR):
  git add BowlingTDD/Game.h BowlingTDD/Game.cpp
  메시지: refactor(game): Apply const noexcept and extract frame constants

  Add kTotalFrames constant, apply const/noexcept throughout Game class.

git push origin master
```

---

## tdd-orchestrator 실행 흐름 요약

```
[초기화]
  현재 상태 확인: main.cpp 더미 테스트 존재 확인
  → 시나리오 1 RED 에이전트에게 main.cpp 정리 포함 지시
  → 각 Phase 별 상세 계획은 doc/Phase{N}_PLAN.md 참조

[시나리오 루프: N = 1..5]
  STEP 1  red-agent 호출
            → 전달: doc/PhaseN_PLAN.md STEP 1 내용
  STEP 2  build-git-agent 호출 (RED 검증)
            → 예상: 컴파일 성공 + 신규 테스트 실패
            → 실패(컴파일 오류): red-agent 재호출 (최대 3회)
            → 실패(이미 통과): RED 무효 처리 후 STEP 5로 이동
  STEP 3  green-agent 호출
            → 전달: doc/PhaseN_PLAN.md STEP 3 내용 + 실패 메시지 전문
  STEP 4  build-git-agent 호출 (GREEN 검증)
            → 예상: 전체 N건 통과
            → 실패: green-agent 재호출 (최대 3회)
  STEP 5  reviewer-agent 호출 ← NEW
            → 전달: doc/PhaseN_PLAN.md STEP 5 내용 + GREEN 코드 요약
            → 반환: "리팩터링 필요 (항목)" 또는 "리팩터링 불필요"
  STEP 6  [REVIEWER 판정이 "필요"일 때만] refactor-agent 호출
            → 전달: reviewer-agent 출력 전문
  STEP 7  [STEP 6 실행 시만] build-git-agent 호출 (REFACTOR 검증)
            → 예상: 전체 N건 통과
            → 실패: refactor-agent 재호출 (최대 2회)
  STEP 8  사용자 검토 요청 (대기)
  STEP 9  사용자 승인 → build-git-agent 커밋·푸시 실행

[완료]
  5/5 시나리오 완료, 총 커밋 수 보고
```

---

## 에러 발생 시 재시도 전략

### 컴파일/링커 오류

1. build-git-agent 가 오류 메시지 전문을 에이전트에게 전달
2. 해당 에이전트(red/green/refactor)가 오류 원인 분석 후 수정
3. 최대 3회 재시도
4. 3회 초과 시 tdd-orchestrator 가 사용자에게 오류 로그 제시하고 대기

### 기존 테스트 회귀

1. 회귀된 테스트 이름과 실패 메시지를 현재 단계 에이전트에게 전달
2. "기존 테스트가 깨졌다. 수정 전 코드로 복구 후 다시 시도하라"는 지시 포함
3. 최대 3회 재시도

### REFACTOR 후 회귀

1. refactor-agent 에게 "리팩터링을 되돌리고 GREEN 상태로 복구"를 지시
2. 복구 후 재리팩터링 시도 (이번에는 더 작은 단위로)
3. 2회 시도 후에도 실패 시 "변경 불필요"로 강제 확정

---

## 커밋 메시지 전체 템플릿

| Phase | RED | GREEN | REVIEW 판정 | REFACTOR |
|-------|-----|-------|-------------|----------|
| 1 거터 게임 | `test(game): Add failing test for gutter game` | `feat(game): Implement gutter game score` | 불필요 | 스킵 |
| 2 올 원 | `test(game): Add failing test for all-ones game` | `feat(game): Accumulate rolls and sum for score` | 가능 | `refactor(game): Rename loop variable to roll_index` |
| 3 스페어 | `test(game): Add failing test for spare bonus` | `feat(game): Add spare bonus to frame-based score` | 필요 | `refactor(game): Extract isSpare helper method` |
| 4 스트라이크 | `test(game): Add failing test for strike bonus` | `feat(game): Add strike bonus to score calculation` | **필요** | `refactor(game): Apply const noexcept to Game methods` |
| 5 퍼펙트 | `test(game): Add perfect game test (passes with existing impl)` | 스킵 | **필요** | `refactor(game): Apply const noexcept and extract frame constants` |

> REFACTOR 커밋은 **REVIEWER 판정이 "필요"이고 실제 코드 변경이 있을 때만** 생성한다.  
> 모든 커밋에 `Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>` footer 포함.

---

## 핵심 파일 경로 참조

| 파일 | 역할 |
|------|------|
| `BowlingTDD/main.cpp` | 테스트 진입점 (InitGoogleMock + RUN_ALL_TESTS) |
| `BowlingTDD/BowlingTDD.vcxproj` | ClCompile ItemGroup 관리 |
| `BowlingTDD/test_bowling.cpp` | 테스트 파일 (신규 생성) |
| `BowlingTDD/Game.h` | Game 클래스 선언 (신규 생성) |
| `BowlingTDD/Game.cpp` | Game 클래스 구현 (신규 생성) |
| `doc/Bowling-KATA.md` | Kata 정의 및 채점 규칙 |
| `doc/COMMIT_CONVENTION.md` | 커밋 메시지 형식 |
| `.claude/skills/test-driven-development/SKILL.md` | TDD 원칙 |
