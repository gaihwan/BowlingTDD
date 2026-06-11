# Phase 5 — 퍼펙트 게임 (Perfect Game) 세부 실행 계획

> **담당 에이전트**: red-agent → build-git-agent → (green-agent) → build-git-agent → reviewer-agent → (refactor-agent) → build-git-agent  
> **목표**: `roll(10)` × 12 → `score() == 300`  
> **전제 조건**: Phase 4 완료 (4건 PASS, score() 스트라이크/스페어 로직 완성)

---

## 진입 시 파일 상태

```
BowlingTDD/
  test_bowling.cpp  ← 4건 테스트 포함
  Game.h            ← isStrike(int), isSpare(int) 선언 (const/noexcept 적용 가능)
  Game.cpp          ← score() 3분기 프레임 루프 + isStrike() + isSpare()
```

---

## ⚠️ 중요: RED 즉시 통과 케이스

Phase 4 구현이 완전하다면 `roll(10) × 12 → score() == 300` 이 **이미 통과**할 수 있다.  
이는 TDD의 자연스러운 진행이다.

- build-git-agent 가 "RED 무효 — 이미 통과" 판정 시  
  → `green-agent` 호출을 **스킵**  
  → `STEP 5 (reviewer-agent)` 로 바로 이동

---

## STEP 1 — RED (red-agent)

### 에이전트: `red-agent`

### 전달 내용

```
시나리오: Phase 5 — 퍼펙트 게임
목표: roll(10) × 12 → score() == 300

기존 통과 테스트:
  - BowlingGameTest::GutterGameScoresZero
  - BowlingGameTest::AllOnesScoresTwenty
  - BowlingGameTest::OneSpareAddsNextRollAsBonus
  - BowlingGameTest::OneStrikeAddsNextTwoRollsAsBonus

[작업] test_bowling.cpp 에 아래 테스트를 추가한다 (기존 테스트 건드리지 않음):

TEST(BowlingGameTest, PerfectGameScoresThreeHundred) {
    Game game;
    for (int i = 0; i < 12; ++i)
        game.roll(10);
    ASSERT_EQ(300, game.score());
}

Game.h / Game.cpp 수정 금지.
rolls_[21] 에 12개 저장 가능 (0~11 인덱스 사용 → 배열 범위 내).
현재 구현으로 통과할 수도 있고 아닐 수도 있음.

참조: doc/Bowling-KATA.md, .claude/skills/test-driven-development/SKILL.md
```

---

## STEP 2 — RED 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: RED 검증 — Phase 5 (퍼펙트 게임)
예상 결과:
  [  OK  ]     BowlingGameTest.GutterGameScoresZero
  [  OK  ]     BowlingGameTest.AllOnesScoresTwenty
  [  OK  ]     BowlingGameTest.OneSpareAddsNextRollAsBonus
  [  OK  ]     BowlingGameTest.OneStrikeAddsNextTwoRollsAsBonus
  [  FAILED  ] BowlingGameTest.PerfectGameScoresThreeHundred (예상)
               또는 PASS (이미 통과 케이스)
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일 오류 | ❌ | red-agent 재호출 (최대 3회) |
| 기존 테스트 회귀 | ❌ | red-agent 수정 요청 |
| PerfectGameScoresThreeHundred **FAIL** (Expected 300) | ✅ RED | STEP 3 (green-agent) 진행 |
| PerfectGameScoresThreeHundred **PASS** | ✅ RED 무효 (이미 통과) | STEP 3 스킵 → STEP 5 진행 |

---

## STEP 3 — GREEN (green-agent) ← 조건부

> **Phase 4 구현으로 이미 통과한 경우 이 단계를 스킵한다.**

### 에이전트: `green-agent`

### 전달 내용 (테스트가 실패한 경우만)

```
시나리오: Phase 5 — 퍼펙트 게임
실패 테스트: BowlingGameTest::PerfectGameScoresThreeHundred
실패 메시지: Expected 300, Actual <잘못된 값>
기존 통과 테스트 4건 유지 필수

점검 사항:
  1. rolls_[21] 배열: 12구 스트라이크 저장 가능 (인덱스 0~11)
  2. score() 루프에서 roll_index 가 최대 얼마나 증가하는가?
     - 10프레임 × 스트라이크 → roll_index += 1 × 10 = 10번째 프레임 끝에 10
     - 10번 프레임에서 roll_index + 1, roll_index + 2 접근 → 최대 12
     - rolls_[21] 범위 내 (0~12 < 21) → 배열 범위 초과 없음
  3. 추가 코드가 필요하다면 최소한만 변경한다.
```

---

## STEP 4 — GREEN 검증 (build-git-agent)

> **STEP 3 스킵 시에도 실행** — 이미 통과 케이스 확인을 위해

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: GREEN 검증 — Phase 5 (퍼펙트 게임)
예상 결과: 전체 5건 통과
  [  OK  ] BowlingGameTest.GutterGameScoresZero
  [  OK  ] BowlingGameTest.AllOnesScoresTwenty
  [  OK  ] BowlingGameTest.OneSpareAddsNextRollAsBonus
  [  OK  ] BowlingGameTest.OneStrikeAddsNextTwoRollsAsBonus
  [  OK  ] BowlingGameTest.PerfectGameScoresThreeHundred
  [  PASSED  ] 5 tests.
```

---

## STEP 5 — REVIEW (reviewer-agent)

### 에이전트: `reviewer-agent`

### 전달 내용

```
시나리오: Phase 5 GREEN 완료 (또는 이미 통과)
리뷰 대상:
  - BowlingTDD/Game.h
  - BowlingTDD/Game.cpp

이것은 Bowling Kata의 마지막 시나리오이자 전체 구현 완성 시점이다.
Phase 4에서 적용하지 못한 최종 품질 점검을 이 단계에서 수행한다.

특별 점검 항목:
  1. const/noexcept 전면 점검:
     - void roll(int pins) noexcept
     - int score() const noexcept
     - bool isStrike(int roll_index) const noexcept
     - bool isSpare(int roll_index) const noexcept
  2. 매직 넘버 상수화:
     - 프레임 루프 상한 10 → static constexpr int kTotalFrames = 10;
  3. 배열 크기 주석:
     - rolls_[21] → // 10 frames × 2 rolls + 1 bonus = 21 max
  4. 헤더-구현 분리 최종 확인
  5. 불필요한 주석/코드 없는지 확인

현재 통과 테스트: 5건
```

### 예상 판정

**리팩터링 필요** — 최종 단계에서 const/noexcept 미적용 또는 매직 넘버 상수화 미완료가 예상된다.

### 판정에 따른 분기

| 판정 | 처리 |
|------|------|
| 리팩터링 불필요 | STEP 6 스킵 → STEP 7 |
| 리팩터링 필요 | STEP 6 진행 |

---

## STEP 6 — REFACTOR (refactor-agent) ← 조건부

### 에이전트: `refactor-agent`

### 전달 내용 (reviewer 판정이 "필요"일 때)

```
시나리오: Phase 5 REFACTOR — 최종 정리
reviewer-agent 지적 사항: <reviewer 출력 내용 전달>

예상 수정 항목 (reviewer 지적 기준으로 실행):
  [항목 1] const/noexcept 전면 적용
    Game.h:
      void roll(int pins) noexcept;
      int  score() const noexcept;
    private:
      bool isStrike(int roll_index) const noexcept;
      bool isSpare(int roll_index)  const noexcept;
    Game.cpp: 모든 정의에 const noexcept 반영

  [항목 2] 매직 넘버 상수화
    Game.h private 섹션에 추가:
      static constexpr int kTotalFrames = 10;
    Game.cpp score() 루프:
      for (int frame = 0; frame < kTotalFrames; ++frame)

  [항목 3] 배열 크기 주석
    Game.h:
      int rolls_[21]{};  // 10 frames × 2 rolls + 1 bonus = 21 max

주의:
  - 한 번에 한 항목씩 변경 후 빌드 검증 권장
  - 기존 5건 통과 유지 필수
  - 동작 변경 없이 구조·이름 개선만
```

### REFACTOR 검증 (build-git-agent)

```
단계: REFACTOR 검증 — Phase 5 (최종)
예상 결과: 전체 5건 통과
REFACTOR 전후 동일한 테스트 결과 확인
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 회귀 발생 | ❌ | refactor-agent 복구 요청 (최대 2회) |
| 전체 5건 통과 | ✅ | STEP 7 진행 |

---

## STEP 7 — 커밋 (build-git-agent)

> 사용자 검토 요청 후 승인 시 실행

### 사용자 검토 요청 형식

```
[사용자 검토 요청 — Phase 5: 퍼펙트 게임]
─────────────────────────────────────
빌드:   ✅ 성공
테스트: ✅ 전체 5건 통과 (Bowling Kata 완성!)
─────────────────────────────────────
변경된 파일:
  [RED]      test_bowling.cpp — PerfectGameScoresThreeHundred 추가
  [GREEN]    <변경 없음 (이미 통과) 또는 변경 요약>
  [REVIEW]   <reviewer 판정 요약>
  [REFACTOR] <변경 요약 또는 "변경 없음">

예정 커밋:
  test(game): Add perfect game test (passes with existing impl)
              또는
              test(game): Add failing test for perfect game
  feat(game): <있을 때만>
  refactor(game): Apply const noexcept and extract frame constants
```

### 커밋 명령

```powershell
# RED 커밋 — 이미 통과 케이스
git add BowlingTDD/test_bowling.cpp
git commit -m "$(cat <<'EOF'
test(game): Add perfect game test (passes with existing impl)

roll(10) x12 scores 300; strike logic from Phase 4 handles it.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# GREEN 커밋 (추가 구현이 있을 때만)
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
feat(game): Fix perfect game boundary in score loop

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# REFACTOR 커밋 (변경이 있을 때만)
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
refactor(game): Apply const noexcept and extract frame constants

Add kTotalFrames = 10 constant, apply const/noexcept throughout
Game class, document rolls_ array size.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

git push origin master
```

---

## Phase 5 완료 후 최종 상태

```
BowlingTDD/
  test_bowling.cpp  ← 5건의 테스트 (Bowling Kata 완성)
  Game.h            ← 완성된 선언
                       void roll(int pins) noexcept;
                       int  score() const noexcept;
                    private:
                       static constexpr int kTotalFrames = 10;
                       int rolls_[21]{};  // 10 frames × 2 rolls + 1 bonus
                       int current_roll_ = 0;
                       bool isStrike(int roll_index) const noexcept;
                       bool isSpare(int roll_index)  const noexcept;
  Game.cpp          ← 완성된 구현

통과 테스트:
  ✅ BowlingGameTest::GutterGameScoresZero
  ✅ BowlingGameTest::AllOnesScoresTwenty
  ✅ BowlingGameTest::OneSpareAddsNextRollAsBonus
  ✅ BowlingGameTest::OneStrikeAddsNextTwoRollsAsBonus
  ✅ BowlingGameTest::PerfectGameScoresThreeHundred

🎳 Bowling Kata TDD 완료!
```
