# Phase 2 — 올 원 (All Ones) 세부 실행 계획

> **담당 에이전트**: red-agent → build-git-agent → green-agent → build-git-agent → reviewer-agent → (refactor-agent) → build-git-agent  
> **목표**: `roll(1)` × 20 → `score() == 20`  
> **전제 조건**: Phase 1 완료 (GutterGameScoresZero PASS, score() returns 0)

---

## 진입 시 파일 상태

```
BowlingTDD/
  test_bowling.cpp  ← GutterGameScoresZero 포함
  Game.h            ← void roll(int), int score() 선언
  Game.cpp          ← roll() 빈 구현, score() { return 0; }
```

---

## STEP 1 — RED (red-agent)

### 에이전트: `red-agent`

### 전달 내용

```
시나리오: Phase 2 — 올 원
목표: roll(1) × 20 → score() == 20

기존 테스트 파일: BowlingTDD/test_bowling.cpp
기존 통과 테스트: BowlingGameTest::GutterGameScoresZero

[작업] test_bowling.cpp 에 아래 테스트를 추가한다 (기존 테스트는 건드리지 않는다):

TEST(BowlingGameTest, AllOnesScoresTwenty) {
    Game game;
    rollMany(game, 20, 1);
    ASSERT_EQ(20, game.score());
}

Game.h / Game.cpp 수정 금지.
현재 score()는 0을 반환하므로 ASSERT_EQ(20, game.score())는 올바른 이유로 실패한다.

참조: doc/Bowling-KATA.md, .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `test_bowling.cpp` 에 `AllOnesScoresTwenty` 추가
- `Game.h` / `Game.cpp` 변경 없음
- 기존 `GutterGameScoresZero` 코드 미변경

---

## STEP 2 — RED 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: RED 검증 — Phase 2 (올 원)
작업 디렉토리: D:\02_Study\02_ClaudeCode\reviewer\TAKA\BowlingTDD

빌드 및 테스트 실행.

예상 결과:
  [  OK  ]     BowlingGameTest.GutterGameScoresZero   ← 기존 테스트 유지
  [  FAILED  ] BowlingGameTest.AllOnesScoresTwenty
  Expected: 20
  Actual:   0
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일 오류 | ❌ | red-agent 재호출 (최대 3회) |
| GutterGameScoresZero FAIL | ❌ | red-agent 에게 기존 테스트 회귀 수정 요청 |
| AllOnesScoresTwenty PASS | ❌ RED 무효 | red-agent 에게 테스트 재작성 요청 |
| GutterGameScoresZero OK + AllOnesScoresTwenty FAIL (Expected 20, Actual 0) | ✅ | STEP 3 진행 |

---

## STEP 3 — GREEN (green-agent)

### 에이전트: `green-agent`

### 전달 내용

```
시나리오: Phase 2 — 올 원
실패 테스트: BowlingGameTest::AllOnesScoresTwenty
실패 메시지: Expected 20, Actual 0
기존 통과 테스트: BowlingGameTest::GutterGameScoresZero (유지 필수)

수정 파일: BowlingTDD/Game.h, BowlingTDD/Game.cpp

Game.h 에 private 멤버 추가:
  int rolls_[21]{};
  int current_roll_ = 0;

Game.cpp 구현:
  void Game::roll(int pins) {
      rolls_[current_roll_++] = pins;
  }
  int Game::score() {
      int total = 0;
      for (int i = 0; i < 20; ++i)
          total += rolls_[i];
      return total;
  }

두 테스트 모두 통과해야 한다.
참조: .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `Game.h` 에 `rolls_[21]`, `current_roll_` 추가
- `Game.cpp` 에 `roll()` 저장 로직, `score()` 합산 로직 구현
- 불필요한 로직(스페어/스트라이크 등) 추가 없음

---

## STEP 4 — GREEN 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: GREEN 검증 — Phase 2 (올 원)
예상 결과: 전체 2건 통과
  [  OK  ] BowlingGameTest.GutterGameScoresZero
  [  OK  ] BowlingGameTest.AllOnesScoresTwenty
  [  PASSED  ] 2 tests.
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일/링커 오류 | ❌ | green-agent 재호출 (최대 3회) |
| 임의 테스트 실패 | ❌ | 실패 내용 전달 후 green-agent 재호출 |
| 전체 2건 통과 | ✅ | STEP 5 진행 |

---

## STEP 5 — REVIEW (reviewer-agent)

### 에이전트: `reviewer-agent`

### 전달 내용

```
시나리오: Phase 2 GREEN 완료
리뷰 대상:
  - BowlingTDD/Game.h  (rolls_[21], current_roll_ 추가됨)
  - BowlingTDD/Game.cpp (roll() 저장, score() 루프 합산)

GREEN 구현 요약:
  - roll(): rolls_[current_roll_++] = pins
  - score(): for i in 0..19: total += rolls_[i]; return total

현재 통과 테스트: GutterGameScoresZero, AllOnesScoresTwenty (2건)
```

### 예상 리뷰 포인트

| 항목 | 내용 | 판정 기준 |
|------|------|-----------|
| 루프 변수명 | `i` 대신 `roll_index` 가 더 명확 | 필요 가능 |
| const 미적용 | `score()` 에 `const` 없음 | 필요 (선택적) |
| 매직 넘버 | `20` — 이 단계에서는 허용 가능 | 미검토 |

### 판정에 따른 분기

| 판정 | 처리 |
|------|------|
| 리팩터링 불필요 | STEP 6 스킵 → STEP 7 (커밋) |
| 리팩터링 필요 | STEP 6 (refactor-agent) 호출 |

---

## STEP 6 — REFACTOR (refactor-agent) ← 조건부

### 에이전트: `refactor-agent`

### 전달 내용 (reviewer-agent 판정이 "필요"일 때)

```
시나리오: Phase 2 REFACTOR
reviewer-agent 지적 사항: <reviewer 출력 내용 전달>

수정 가이드 (reviewer 지적 항목 기준으로 실행):
  예) 루프 변수 i → roll_index 로 변경
  예) score() 에 const 추가 (Game.h 선언에도 반영)

기존 통과 테스트 2건 유지 필수
동작 변경 없이 이름/구조만 개선
```

### REFACTOR 검증 (build-git-agent)

```
단계: REFACTOR 검증 — Phase 2
예상 결과: 전체 2건 통과 (변경 전후 동일)
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 회귀 발생 | ❌ | refactor-agent 에게 복구 요청 |
| 전체 2건 통과 | ✅ | STEP 7 진행 |

---

## STEP 7 — 커밋 (build-git-agent)

> 사용자 검토 요청 후 승인 시 실행

### 사용자 검토 요청 형식

```
[사용자 검토 요청 — Phase 2: 올 원]
─────────────────────────────────────
빌드:   ✅ 성공
테스트: ✅ 전체 2건 통과
─────────────────────────────────────
변경된 파일:
  [RED]      test_bowling.cpp — AllOnesScoresTwenty 추가
  [GREEN]    Game.h           — rolls_[21], current_roll_ 추가
  [GREEN]    Game.cpp         — roll() 저장, score() 합산
  [REVIEW]   <reviewer 판정 요약>
  [REFACTOR] <변경 요약 또는 "변경 없음">

예정 커밋:
  test(game): Add failing test for all-ones game
  feat(game): Accumulate rolls and sum for score
  refactor(game): <subject>  ← REFACTOR 변경 있을 때만
```

### 커밋 명령

```powershell
# RED 커밋
git add BowlingTDD/test_bowling.cpp
git commit -m "$(cat <<'EOF'
test(game): Add failing test for all-ones game

ASSERT_EQ(20, game.score()) fails because score() returns 0.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# GREEN 커밋
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
feat(game): Accumulate rolls and sum for score

Introduce rolls_ array and current_roll_ to track each roll;
score() sums all rolls to support any pin count.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# REFACTOR 커밋 (변경이 있을 때만)
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
refactor(game): Rename loop variable to roll_index

Replace single-char loop index i with roll_index for clarity.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

git push origin master
```

---

## Phase 2 완료 후 상태

```
BowlingTDD/
  test_bowling.cpp  ← GutterGameScoresZero + AllOnesScoresTwenty
  Game.h            ← rolls_[21], current_roll_ private 멤버 추가
  Game.cpp          ← roll() 저장, score() 합산 루프

통과 테스트:
  ✅ BowlingGameTest::GutterGameScoresZero
  ✅ BowlingGameTest::AllOnesScoresTwenty

다음 단계: Phase3_PLAN.md (스페어 한 번)
```
