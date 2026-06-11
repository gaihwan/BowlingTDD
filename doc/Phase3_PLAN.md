# Phase 3 — 스페어 한 번 (One Spare) 세부 실행 계획

> **담당 에이전트**: red-agent → build-git-agent → green-agent → build-git-agent → reviewer-agent → (refactor-agent) → build-git-agent  
> **목표**: `roll(5), roll(5), roll(3)`, 나머지 0 → `score() == 16`  
> **전제 조건**: Phase 2 완료 (2건 PASS, score() 단순 합산 구현)

---

## 진입 시 파일 상태

```
BowlingTDD/
  test_bowling.cpp  ← GutterGameScoresZero, AllOnesScoresTwenty 포함
  Game.h            ← rolls_[21], current_roll_ 선언
  Game.cpp          ← roll() 저장, score() 단순 합산 (0~19 인덱스)
```

---

## STEP 1 — RED (red-agent)

### 에이전트: `red-agent`

### 전달 내용

```
시나리오: Phase 3 — 스페어 한 번
목표: 5,5,3,0×17 → score() == 16  (스페어 보너스: 5+5+3=13 아닌 16)

기존 테스트 파일: BowlingTDD/test_bowling.cpp
기존 통과 테스트:
  - BowlingGameTest::GutterGameScoresZero
  - BowlingGameTest::AllOnesScoresTwenty

[작업] test_bowling.cpp 에 아래 테스트를 추가한다 (기존 테스트는 건드리지 않는다):

TEST(BowlingGameTest, OneSpareAddsNextRollAsBonus) {
    Game game;
    rollSpare(game);           // roll(5), roll(5)
    game.roll(3);
    rollMany(game, 17, 0);     // 나머지 0
    ASSERT_EQ(16, game.score());
}

rollSpare 헬퍼가 test_bowling.cpp 의 namespace 블록에 있는지 확인하고 없으면 추가:
  void rollSpare(Game& game) { game.roll(5); game.roll(5); }

Game.h / Game.cpp 수정 금지.
현재 score()는 단순 합산 → 5+5+3=13 반환, 보너스 3 누락.
예상 실패: Expected 16, Actual 13

참조: doc/Bowling-KATA.md, .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `test_bowling.cpp` 에 `OneSpareAddsNextRollAsBonus` 추가
- `rollSpare` 헬퍼 존재 확인 (없으면 추가)
- `Game.h` / `Game.cpp` 변경 없음

---

## STEP 2 — RED 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: RED 검증 — Phase 3 (스페어)
예상 결과:
  [  OK  ]     BowlingGameTest.GutterGameScoresZero
  [  OK  ]     BowlingGameTest.AllOnesScoresTwenty
  [  FAILED  ] BowlingGameTest.OneSpareAddsNextRollAsBonus
  Expected: 16
  Actual:   13
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일 오류 | ❌ | red-agent 재호출 (최대 3회) |
| 기존 테스트 회귀 | ❌ | red-agent 에게 수정 요청 |
| OneSpareAddsNextRollAsBonus PASS | ❌ RED 무효 | red-agent 재작성 요청 |
| 2건 OK + OneSpareAddsNextRollAsBonus FAIL (Expected 16, Actual 13) | ✅ | STEP 3 진행 |

---

## STEP 3 — GREEN (green-agent)

### 에이전트: `green-agent`

### 전달 내용

```
시나리오: Phase 3 — 스페어
실패 테스트: BowlingGameTest::OneSpareAddsNextRollAsBonus
실패 메시지: Expected 16, Actual 13
기존 통과 테스트:
  - BowlingGameTest::GutterGameScoresZero
  - BowlingGameTest::AllOnesScoresTwenty
  (두 테스트 모두 유지 필수)

수정 파일: BowlingTDD/Game.h, BowlingTDD/Game.cpp

Game.h 에 private 메서드 선언 추가:
  bool isSpare(int roll_index);

Game.cpp — score() 를 프레임 단위 루프로 재작성:
  int Game::score() {
      int total = 0;
      int roll_index = 0;
      for (int frame = 0; frame < 10; ++frame) {
          if (isSpare(roll_index)) {
              total += 10 + rolls_[roll_index + 2];
              roll_index += 2;
          } else {
              total += rolls_[roll_index] + rolls_[roll_index + 1];
              roll_index += 2;
          }
      }
      return total;
  }

  bool Game::isSpare(int roll_index) {
      return rolls_[roll_index] + rolls_[roll_index + 1] == 10;
  }

스트라이크 로직 추가 금지 (아직 해당 시나리오 아님).
rolls_[21] 배열 크기 유지.
참조: .claude/skills/test-driven-development/SKILL.md
```

### 성공 판정

- `score()` 프레임 루프 구현
- `isSpare()` 선언/구현
- 3건 모두 통과 예상
- 스트라이크 관련 코드 미포함

---

## STEP 4 — GREEN 검증 (build-git-agent)

### 에이전트: `build-git-agent`

### 전달 내용

```
단계: GREEN 검증 — Phase 3 (스페어)
예상 결과: 전체 3건 통과
  [  OK  ] BowlingGameTest.GutterGameScoresZero
  [  OK  ] BowlingGameTest.AllOnesScoresTwenty
  [  OK  ] BowlingGameTest.OneSpareAddsNextRollAsBonus
  [  PASSED  ] 3 tests.
```

### 판정 기준

| 결과 | 판정 | 처리 |
|------|------|------|
| 컴파일/링커 오류 | ❌ | green-agent 재호출 (최대 3회) |
| 임의 테스트 실패 | ❌ | 실패 내용 전달 후 green-agent 재호출 |
| 전체 3건 통과 | ✅ | STEP 5 진행 |

---

## STEP 5 — REVIEW (reviewer-agent)

### 에이전트: `reviewer-agent`

### 전달 내용

```
시나리오: Phase 3 GREEN 완료
리뷰 대상:
  - BowlingTDD/Game.h  (isSpare 선언 추가됨)
  - BowlingTDD/Game.cpp (score() 프레임 루프, isSpare() 구현)

GREEN 구현 요약:
  - score(): 프레임 루프 10회, isSpare() 감지 시 보너스 roll 추가
  - isSpare(): rolls_[i] + rolls_[i+1] == 10

현재 통과 테스트: 3건
```

### 주요 리뷰 포인트

| 항목 | 확인 내용 | 판정 가능성 |
|------|-----------|-------------|
| isSpare 메서드 분리 | 이미 분리되어 있으면 OK | 구현에 따라 다름 |
| const 미적용 | `score()`, `isSpare()` 에 const 없음 | 필요 가능 |
| 프레임 루프 가독성 | `frame` 변수 사용, `roll_index` 추적 방식 | 대체로 OK |
| 매직 넘버 10 | 프레임 수 10 — 이 단계에서 허용 가능 | 미검토 |

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
시나리오: Phase 3 REFACTOR
reviewer-agent 지적 사항: <reviewer 출력 내용 전달>

수정 가이드 (reviewer 지적 항목 기준):
  예) isSpare() 에 const 추가
  예) score() 에 const 추가
  예) 프레임 루프 변수명 정리

기존 통과 테스트 3건 유지 필수
동작 변경 없이 품질 개선만
```

### REFACTOR 검증 (build-git-agent)

```
단계: REFACTOR 검증 — Phase 3
예상 결과: 전체 3건 통과
```

---

## STEP 7 — 커밋 (build-git-agent)

> 사용자 검토 요청 후 승인 시 실행

### 사용자 검토 요청 형식

```
[사용자 검토 요청 — Phase 3: 스페어 한 번]
─────────────────────────────────────
빌드:   ✅ 성공
테스트: ✅ 전체 3건 통과
─────────────────────────────────────
변경된 파일:
  [RED]      test_bowling.cpp — OneSpareAddsNextRollAsBonus 추가
  [GREEN]    Game.h           — isSpare(int) 선언 추가
  [GREEN]    Game.cpp         — score() 프레임 루프, isSpare() 구현
  [REVIEW]   <reviewer 판정 요약>
  [REFACTOR] <변경 요약 또는 "변경 없음">

예정 커밋:
  test(game): Add failing test for spare bonus
  feat(game): Add spare bonus to frame-based score
  refactor(game): <subject>  ← 변경 있을 때만
```

### 커밋 명령

```powershell
# RED 커밋
git add BowlingTDD/test_bowling.cpp
git commit -m "$(cat <<'EOF'
test(game): Add failing test for spare bonus

ASSERT_EQ(16, game.score()) fails; simple sum returns 13,
missing the spare bonus roll.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# GREEN 커밋
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
feat(game): Add spare bonus to frame-based score

Rewrite score() with 10-frame loop; when spare detected,
add next roll as bonus via isSpare() helper.

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

# REFACTOR 커밋 (변경이 있을 때만)
git add BowlingTDD/Game.h BowlingTDD/Game.cpp
git commit -m "$(cat <<'EOF'
refactor(game): Apply const to isSpare and score methods

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"

git push origin master
```

---

## Phase 3 완료 후 상태

```
BowlingTDD/
  test_bowling.cpp  ← 3건의 테스트 포함
  Game.h            ← isSpare(int) private 선언 추가
  Game.cpp          ← score() 프레임 루프, isSpare() 구현

통과 테스트:
  ✅ BowlingGameTest::GutterGameScoresZero
  ✅ BowlingGameTest::AllOnesScoresTwenty
  ✅ BowlingGameTest::OneSpareAddsNextRollAsBonus

다음 단계: Phase4_PLAN.md (스트라이크 한 번)
```
