# Phase 4 — 스트라이크 한 번 (One Strike) 세부 실행 계획

> **담당 에이전트**: red-agent → build-git-agent → green-agent → build-git-agent → reviewer-agent → (refactor-agent) → build-git-agent  
> **목표**: `roll(10), roll(3), roll(4)`, 나머지 0 → `score() == 24`  
> **전제 조건**: Phase 3 완료 (3건 PASS, score() 프레임 루프 + isSpare() 구현)

---

## 진입 시 파일 상태

```
BowlingTDD/
  test_bowling.cpp  ← 3건 테스트 포함
  Game.h            ← isSpare(int) private 선언
  Game.cpp          ← score() 프레임 루프 + isSpare()
```

---

## STEP 1 — RED (red-agent)

### 에이전트: `red-agent`

### 전달 내용

```
시나리오: Phase 4 — 스트라이크 한 번
목표: roll(10),roll(3),roll(4),0×16 → score() == 24
      스트라이크 보너스: 10 + 3 + 4 = 17, 나머지 프레임 3+4=7 → 총 24

기존 통과 테스트:
  - BowlingGameTest::GutterGameScoresZero
  - BowlingGameTest::AllOnesScoresTwenty
  - BowlingGameTest::OneSpareAddsNextRollAsBonus

[작업] test_bowling.cpp 에 아래 테스트를 추가한다 (기존 테스트 건드리지 않음):

TEST(BowlingGameTest, OneStrikeAddsNextTwoRollsAsBonus) {
    Game game;
    rollStrike(game);          // roll(10)
    game.roll(3);
    game.roll(4);
    rollMany(game, 16, 0);     // 나머지 0 (10프레임까지)
    ASSERT_EQ(24, game.score());
}

rollStrike 헬퍼가 namespace 블록에 있는지 확인하고 없으면 추가:
  void rollStrike(Game& game) { game.roll(10); }

Game.h / Game.cpp 수정 금지.
현재 구현은 스트라이크를 일반 프레임(2구)으로 처리하므로
보너스 계산이 잘못된다. 예상 실패: Expected 24, Actual ≠ 24

참조: doc/Bowling-KATA.md, .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `test_bowling.cpp` 에 `OneStrikeAddsNextTwoRollsAsBonus` 추가
- `rollStrike` 헬퍼 존재 확인
- `Game.h` / `Game.cpp` 변경 없음

---

## STEP 2 — RED 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: RED 검증 — Phase 4 (스트라이크)
예상 결과:
  [  OK  ]     BowlingGameTest.GutterGameScoresZero
  [  OK  ]     BowlingGameTest.AllOnesScoresTwenty
  [  OK  ]     BowlingGameTest.OneSpareAddsNextRollAsBonus
  [  FAILED  ] BowlingGameTest.OneStrikeAddsNextTwoRollsAsBonus
  Expected: 24
  Actual:   <24 아닌 값>
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일 오류 | ❌ | red-agent 재호출 (최대 3회) |
| 기존 테스트 회귀 | ❌ | red-agent 수정 요청 |
| OneStrikeAddsNextTwoRollsAsBonus PASS | ❌ RED 무효 | red-agent 재작성 요청 |
| 3건 OK + 신규 FAIL (Expected 24) | ✅ | STEP 3 진행 |

---

## STEP 3 — GREEN (green-agent)

### 에이전트: `green-agent`

### 전달 내용

```
시나리오: Phase 4 — 스트라이크
실패 테스트: BowlingGameTest::OneStrikeAddsNextTwoRollsAsBonus
실패 메시지: Expected 24, Actual <잘못된 값>
기존 통과 테스트 3건 유지 필수

수정 파일: BowlingTDD/Game.h, BowlingTDD/Game.cpp

Game.h 에 private 메서드 선언 추가:
  bool isStrike(int roll_index);

Game.cpp — score() 에 스트라이크 분기 추가 (스페어 분기보다 먼저):
  int Game::score() {
      int total = 0;
      int roll_index = 0;
      for (int frame = 0; frame < 10; ++frame) {
          if (isStrike(roll_index)) {
              total += 10 + rolls_[roll_index + 1] + rolls_[roll_index + 2];
              roll_index += 1;   // 스트라이크는 1구만 소비
          } else if (isSpare(roll_index)) {
              total += 10 + rolls_[roll_index + 2];
              roll_index += 2;
          } else {
              total += rolls_[roll_index] + rolls_[roll_index + 1];
              roll_index += 2;
          }
      }
      return total;
  }

  bool Game::isStrike(int roll_index) {
      return rolls_[roll_index] == 10;
  }

핵심: 스트라이크 프레임에서 roll_index += 1 (2가 아님).
스트라이크 분기는 반드시 isSpare 분기보다 먼저 위치해야 함.
rolls_[21] 배열 유지.
참조: .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `isStrike()` 선언/구현
- `score()` 에서 스트라이크 분기가 스페어 분기 앞에 위치
- `roll_index += 1` (스트라이크 프레임)
- 4건 모두 통과

---

## STEP 4 — GREEN 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: GREEN 검증 — Phase 4 (스트라이크)
예상 결과: 전체 4건 통과
  [  OK  ] BowlingGameTest.GutterGameScoresZero
  [  OK  ] BowlingGameTest.AllOnesScoresTwenty
  [  OK  ] BowlingGameTest.OneSpareAddsNextRollAsBonus
  [  OK  ] BowlingGameTest.OneStrikeAddsNextTwoRollsAsBonus
  [  PASSED  ] 4 tests.
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일/링커 오류 | ❌ | green-agent 재호출 (최대 3회) |
| 임의 테스트 실패 | ❌ | 실패 내용 전달 후 green-agent 재호출 |
| 전체 4건 통과 | ✅ | STEP 5 진행 |

---

## STEP 5 — REVIEW (reviewer-agent)

### 에이전트: `reviewer-agent`

### 전달 내용

```
시나리오: Phase 4 GREEN 완료
리뷰 대상:
  - BowlingTDD/Game.h  (isStrike, isSpare 선언)
  - BowlingTDD/Game.cpp (score() strike/spare/normal 3분기, isStrike(), isSpare())

GREEN 구현 요약:
  - score(): 프레임 루프, isStrike → 1구 소비/2구 보너스, isSpare → 2구 소비/1구 보너스
  - isStrike(): rolls_[i] == 10
  - isSpare(): rolls_[i] + rolls_[i+1] == 10

현재 통과 테스트: 4건
```

### 주요 리뷰 포인트

| 항목 | 확인 내용 | 판정 가능성 |
|------|-----------|-------------|
| isStrike 분리 | 이미 메서드로 분리되어 있으면 OK | 구현에 따라 다름 |
| const 미적용 | score(), isStrike(), isSpare() | **필요 높음** |
| noexcept 미적용 | roll(), score() | 필요 가능 |
| score() 가독성 | 3분기 if/else if/else 블록 구조 | 대체로 OK |
| roll_index 증감 | 스트라이크 +1, 스페어/일반 +2 | 명확성 확인 |

### 예상 판정

**리팩터링 필요** — `const`/`noexcept` 미적용이 가장 빈번한 지적 포인트다.

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
시나리오: Phase 4 REFACTOR
reviewer-agent 지적 사항: <reviewer 출력 내용 전달>

수정 가이드 (reviewer 지적 항목 기준):
  [예상 항목 1] isStrike(), isSpare() 에 const noexcept 추가
    Game.h:  bool isStrike(int roll_index) const noexcept;
             bool isSpare(int roll_index)  const noexcept;
    Game.cpp: 정의에도 const noexcept 반영
  [예상 항목 2] score() 에 const noexcept 추가
    Game.h:  int score() const noexcept;
  [예상 항목 3] roll() 에 noexcept 추가
    Game.h:  void roll(int pins) noexcept;

주의:
  - const 추가 시 Game.h 선언과 Game.cpp 정의 모두 수정
  - 기존 통과 테스트 4건 유지 필수
```

### REFACTOR 검증 (build-git-agent)

```
단계: REFACTOR 검증 — Phase 4
예상 결과: 전체 4건 통과
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 회귀 발생 | ❌ | refactor-agent 복구 요청 (최대 2회) |
| 전체 4건 통과 | ✅ | STEP 7 진행 |

---

## STEP 7 — 커밋 (build-git-agent)

> 사용자 검토 요청 후 승인 시 실행

### 사용자 검토 요청 형식

```
[사용자 검토 요청 — Phase 4: 스트라이크 한 번]
─────────────────────────────────────
빌드:   ✅ 성공
테스트: ✅ 전체 4건 통과
─────────────────────────────────────
변경된 파일:
  [RED]      test_bowling.cpp — OneStrikeAddsNextTwoRollsAsBonus 추가
  [GREEN]    Game.h           — isStrike(int) 선언 추가
  [GREEN]    Game.cpp         — score() 스트라이크 분기, isStrike() 구현
  [REVIEW]   <reviewer 판정 요약>
  [REFACTOR] <변경 요약 또는 "변경 없음">

예정 커밋:
  test(game): Add failing test for strike bonus
  feat(game): Add strike bonus to score calculation
  refactor(game): Extract isStrike and apply const noexcept  ← 변경 있을 때만
```

### 커밋 명령

```powershell
# RED 커밋
git add BowlingTDD/test_bowling.cpp
git commit -m "$(cat <<'EOF'
test(game): Add failing test for strike bonus

ASSERT_EQ(24, game.score()) fails; strike bonus not implemented.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# GREEN 커밋
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
feat(game): Add strike bonus to score calculation

Detect first-ball 10, add next two rolls as bonus,
advance roll_index by 1 for strike frame.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# REFACTOR 커밋 (변경이 있을 때만)
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
refactor(game): Apply const noexcept to Game methods

Add const to score(), isStrike(), isSpare();
add noexcept to roll(), score(), isStrike(), isSpare().

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

git push origin master
```

---

## Phase 4 완료 후 상태

```
BowlingTDD/
  test_bowling.cpp  ← 4건의 테스트 포함
  Game.h            ← isStrike(int), isSpare(int) private 선언
                       (const/noexcept 적용된 경우 반영)
  Game.cpp          ← score() 3분기 프레임 루프, isStrike(), isSpare()

통과 테스트:
  ✅ BowlingGameTest::GutterGameScoresZero
  ✅ BowlingGameTest::AllOnesScoresTwenty
  ✅ BowlingGameTest::OneSpareAddsNextRollAsBonus
  ✅ BowlingGameTest::OneStrikeAddsNextTwoRollsAsBonus

다음 단계: Phase5_PLAN.md (퍼펙트 게임)
```
