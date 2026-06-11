# TDD 실행 계획 수립 및 REVIEWER Agent 추가 세션

이 문서는 Bowling Kata TDD 프로젝트에서 실행 계획(PLAN.md)을 수립하고,
Phase별 세부 계획 파일을 분리하며, REVIEWER Agent를 추가하는 과정 전체를
사용자 입력 포함하여 시간순으로 기록한다.

---

## 1단계 — 전체 실행 계획 수립 (PLAN.md)

### 사용자 입력

```
tdd-orchestrator는 각 sub agent를 이용하여 TDD Bowling KATA를 어떻게 진행할지에 대한
plan을 세워서 PLAN.md를 작성해줘.
```

### 작업 내용

`tdd-orchestrator` 에이전트가 아래 참조 문서를 읽고 전체 실행 계획을 수립했다.

- `doc/Bowling-KATA.md`
- `doc/COMMIT_CONVENTION.md`
- `.claude/skills/test-driven-development/SKILL.md`
- `CLAUDE.md` (프로젝트 지침)

### 생성된 파일

`PLAN.md` — 프로젝트 루트에 생성

### PLAN.md 주요 구성

#### 현재 상태 파악

| 항목 | 내용 |
|------|------|
| `main.cpp` | 더미 테스트 `SampleTest::AssertionFails` 포함 → 첫 RED에서 제거 필요 |
| `Game.h`, `Game.cpp`, `test_bowling.cpp` | 미존재 → 시나리오 1 RED에서 신규 생성 |
| gtest/gmock | NuGet 패키지 `gmock 1.11.0` 설치됨 |

#### 사이클 구조 (초기)

```
RED → [빌드검증] → GREEN → [빌드검증] → REFACTOR → [빌드검증] → 사용자승인 → 커밋/푸시
```

#### 5개 시나리오 커밋 전략

| 시나리오 | RED 커밋 | GREEN 커밋 | REFACTOR 커밋 |
|----------|----------|------------|---------------|
| 1 거터 게임 | `test(game): Add failing test for gutter game` | `feat(game): Implement gutter game score` | 스킵 |
| 2 올 원 | `test(game): Add failing test for all-ones game` | `feat(game): Accumulate rolls and sum for score` | `refactor(game): Rename loop variable to roll_index` |
| 3 스페어 | `test(game): Add failing test for spare bonus` | `feat(game): Add spare bonus to frame-based score` | `refactor(game): Extract isSpare helper method` |
| 4 스트라이크 | `test(game): Add failing test for strike bonus` | `feat(game): Add strike bonus to score calculation` | `refactor(game): Extract isStrike and apply const noexcept` |
| 5 퍼펙트 | `test(game): Add perfect game test (passes with existing impl)` | 스킵(이미 통과 예상) | `refactor(game): Apply const noexcept and extract frame constants` |

#### 에러 처리 전략

- 각 에이전트 재시도 상한: **3회**
- 퍼펙트 게임 RED 즉시 통과 시: GREEN 스킵, RED 커밋에 `(passes with existing impl)` 명시
- REFACTOR 후 회귀 시: 복구 요청 2회 후 "변경 불필요" 강제 확정

---

## 2단계 — Phase별 세부 계획 분리 + REVIEWER Agent 추가

### 사용자 입력

```
RED 전문가의 역할에 tdd-orchestrator가 만든 PLAN.md에 의거하여
RED->GREEN->REFACTOR->BUILD Cycle의 세부 계획을 세우고 이 계획에 따라
세부 계획은 Phase1 PLAN.md Phase2 PLAN.md 식으로 세부 PLAN을 작성하고,
다른 agent는 이 계획에 따라 Phase (Cycle)을 수행할 수 있도록 해줘.
그리고 REFACTOR외에 REVIEWER Agent를 추가하고 이 Reviewer Agent는
Green 전문가가 만들어낸 코드를 리뷰하고 refactoring이 필요하면
refactoring 전문가에게 refactoring을 요청할 수 있도록 Cycle내 단계를 추가해줘.
```

### 핵심 요구사항

1. **Phase별 세부 PLAN 파일 분리** — 각 에이전트가 독립 참조 가능한 자기완결형 문서
2. **REVIEWER Agent 신규 추가** — GREEN과 REFACTOR 사이에 삽입
3. **REVIEWER의 역할**: 코드를 직접 수정하지 않고 리뷰 후 REFACTOR 필요 여부만 판정
4. **기존 에이전트 업데이트** — tdd-orchestrator, build-git-agent에 REVIEWER 단계 반영

---

## 3단계 — 신규 생성 파일

### `reviewer_agent.md`

```
경로: .claude/agents/reviewer_agent.md
```

**역할 정의**

GREEN 단계 완료 직후 `Game.h`, `Game.cpp` 를 리뷰한다.  
코드를 직접 수정하지 않는다. 판정만 내린다.

**6가지 리뷰 체크리스트**

| 번호 | 항목 | 설명 |
|------|------|------|
| 1 | 중복 (DRY 위반) | 동일 조건이 2곳 이상 반복되는가 |
| 2 | 이름 명확성 | 단일 문자 변수, 매직 넘버, 의도 불명확한 이름 |
| 3 | `const`/`noexcept` | 상태 비변경 메서드에 const 없음, 예외 불필요 메서드에 noexcept 없음 |
| 4 | 함수 크기·책임 | score() 내 판별 로직 인라인 여부, 20줄 초과 여부 |
| 5 | 헤더-구현 분리 | 구현이 .h 파일에 있는가 |
| 6 | 불필요한 코드 | 미사용 변수, 미래 대비 코드 |

**"리팩터링 면제" 조건**

위 6가지를 **모두** 통과하면 "리팩터링 불필요" 판정.  
단, Phase 1의 `score() { return 0; }` 수준은 의도적 최소 구현으로 면제.

**출력 형식 — 필요 시**

```
[REVIEW 완료 — 리팩터링 필요]
발견된 개선 항목:
  ❶ [중복] score() 내 rolls_[roll_index] == 10 조건 인라인 중복
     → isStrike(int roll_index) private 메서드 추출 권고
  ❷ [const] score()에 const 미적용
  ...
refactor-agent에게 위임 요청: 위 항목 개선, 모든 테스트 통과 유지
```

**출력 형식 — 불필요 시**

```
[REVIEW 완료 — 리팩터링 불필요]
판정 이유: 현재 구현이 이 시나리오 단계에서 최소하고 명확하다.
→ REFACTOR 단계 스킵, 커밋 단계로 진행
```

**절대 금지사항**

- 코드 파일 직접 수정
- 코드 스니펫 생성 (방향만 서술)
- 미구현 시나리오를 위한 선제적 개선 요구
- 테스트 코드 리뷰 (프로덕션 코드만)

---

### Phase별 PLAN 파일 (5개)

```
doc/Phase1_PLAN.md   ← 거터 게임
doc/Phase2_PLAN.md   ← 올 원
doc/Phase3_PLAN.md   ← 스페어 한 번
doc/Phase4_PLAN.md   ← 스트라이크 한 번
doc/Phase5_PLAN.md   ← 퍼펙트 게임
```

**각 파일의 구조** (공통)

```
# Phase N — <시나리오명> 세부 실행 계획

진입 시 파일 상태          ← 이전 Phase 완료 후 상태
STEP 1 — RED              ← 에이전트 이름 + 전달 내용 전문
STEP 2 — RED 검증         ← 판정 기준 표
STEP 3 — GREEN            ← 에이전트 이름 + 전달 내용 전문
STEP 4 — GREEN 검증       ← 판정 기준 표
STEP 5 — REVIEW           ← 전달 내용 + 예상 리뷰 포인트 + 분기
STEP 6 — REFACTOR (조건부) ← reviewer 판정 "필요"일 때만 실행
STEP 7 — 커밋              ← 사용자 검토 요청 형식 + git 명령 전문
Phase N 완료 후 상태       ← 다음 Phase 진입 조건
```

**자기완결형 설계 원칙**

각 Phase PLAN은 해당 에이전트에게 **그대로 복사해 전달**할 수 있는 지시 블록을 포함한다.  
tdd-orchestrator가 별도 판단 없이 해당 STEP의 "전달 내용"을 그대로 사용한다.

**Phase별 예상 REVIEWER 판정**

| Phase | 예상 판정 | 주요 리뷰 포인트 |
|-------|-----------|-----------------|
| 1 거터 게임 | 불필요 | `score() { return 0; }` — 의도적 최소 구현 |
| 2 올 원 | 가능 | 루프 변수 `i` → `roll_index` |
| 3 스페어 | 필요 가능 | `isSpare()` 인라인 여부, `const` 미적용 |
| 4 스트라이크 | **필요** | `isStrike()` 인라인, `const`/`noexcept` 미적용 |
| 5 퍼펙트 | **필요** | 전체 `const`/`noexcept`, 매직 넘버 상수화 |

---

## 4단계 — 기존 에이전트 파일 업데이트

### `tdd_orchestrator.md` 변경사항

**description 업데이트**
```
Before: RED → GREEN → REFACTOR 단계를 순서대로 위임
After:  RED → GREEN → REVIEW → (REFACTOR) 단계를 순서대로 위임
```

**Kata 진행 순서 테이블** — "상세 계획" 컬럼 추가

| 순서 | 시나리오 | 상세 계획 |
|------|----------|-----------|
| 1~5 | ... | `doc/Phase{N}_PLAN.md` |

**한 사이클 진행 절차** — REVIEWER 단계 삽입

```
(기존)  RED → BUILD → GREEN → BUILD → REFACTOR → BUILD → 커밋
(변경)  RED → BUILD → GREEN → BUILD → REVIEW → (REFACTOR → BUILD) → 커밋
```

**REVIEWER → REFACTOR 연결 규칙 추가**

```
REVIEWER 판정: "필요"  →  개선 항목을 REFACTOR에 전달 → REFACTOR 실행 → BUILD 검증
REVIEWER 판정: "불필요" →  REFACTOR 스킵 → 커밋 단계로 이동
```

**중단 조건 추가**

```
REVIEWER → REFACTOR → BUILD 사이클이 2회 반복 후에도 회귀 발생 시 사용자 보고
```

---

### `build_agent.md` 변경사항

**description 업데이트**
```
Before: RED / GREEN / REFACTOR 각 단계에서 ...
After:  RED / GREEN / REVIEW / REFACTOR 각 단계에서 ...
```

**한 사이클 처리 흐름** — REVIEWER 판정 수신 분기 추가

```
GREEN Pass → REVIEWER 에이전트 호출 승인
  ├─ "리팩터링 필요"  → REFACTOR 에이전트 호출 승인
  └─ "리팩터링 불필요" → 커밋 단계로 이동
```

**GREEN 검증 판정 기준 변경**

```
Before: 전체 통과 → REFACTOR 에이전트 호출 승인
After:  전체 통과 → REVIEWER 에이전트 호출 승인
```

**사용자 검토 요청 형식** — REVIEW 항목 추가

```
[REVIEW] <리뷰 판정: 필요(항목 요약) / 불필요>
```

---

## 5단계 — PLAN.md 업데이트

마스터 계획 PLAN.md 에 다음 내용을 추가/변경했다.

### 추가된 섹션

**문서 구조 섹션** (최상단)

```
PLAN.md               ← 전체 개요, 에이전트 구성, 사이클 흐름
doc/Phase1_PLAN.md    ← 각 에이전트 전달 내용 포함된 세부 계획
...
doc/Phase5_PLAN.md
```

**TDD 사이클 다이어그램** (업데이트)

```
RED → [빌드검증] → GREEN → [빌드검증] → REVIEW → (필요 시) REFACTOR → [빌드검증] → 커밋
                                               ↑
                                        reviewer-agent가
                                        GREEN 코드를 리뷰하고
                                        REFACTOR 필요 여부 판정
```

**에이전트 구성표** (신규)

| 에이전트 | 역할 | 정의 파일 |
|----------|------|-----------|
| `red-agent` | 실패하는 테스트 작성 | `red_agent.md` |
| `green-agent` | 최소 프로덕션 코드 구현 | `green_agent.md` |
| `reviewer-agent` | GREEN 코드 리뷰 → REFACTOR 필요 여부 판정 | `reviewer_agent.md` |
| `refactor-agent` | REVIEWER 지적 사항 기반 코드 품질 개선 | `refactor_agent.md` |
| `build-git-agent` | 빌드·검증·Git 커밋·푸시 | `build_agent.md` |

### 변경된 섹션

**tdd-orchestrator 실행 흐름** — STEP 5(REVIEW), STEP 6(REFACTOR 조건부), STEP 7(REFACTOR 검증 조건부)로 재편

**커밋 메시지 테이블** — REVIEW 판정 컬럼 추가

---

## 최종 파일 구조

```
BowlingTDD/
├── PLAN.md                              ← 마스터 계획 (업데이트)
├── CLAUDE.md
├── doc/
│   ├── Bowling-KATA.md
│   ├── COMMIT_CONVENTION.md
│   ├── Phase1_PLAN.md                   ← 신규 (거터 게임)
│   ├── Phase2_PLAN.md                   ← 신규 (올 원)
│   ├── Phase3_PLAN.md                   ← 신규 (스페어)
│   ├── Phase4_PLAN.md                   ← 신규 (스트라이크)
│   └── Phase5_PLAN.md                   ← 신규 (퍼펙트 게임)
├── lecture_docs/
│   ├── tdd 사전 claude 세팅.md
│   ├── sub agent 설정.md
│   └── tdd_plan_and_reviewer_agent.md   ← 이 파일
└── .claude/
    ├── agents/
    │   ├── tdd_orchestrator.md           ← 업데이트 (REVIEWER 단계 추가)
    │   ├── red_agent.md
    │   ├── green_agent.md
    │   ├── refactor_agent.md
    │   ├── reviewer_agent.md             ← 신규
    │   └── build_agent.md               ← 업데이트 (REVIEWER 분기 반영)
    └── skills/
        └── test-driven-development/
            └── SKILL.md
```

---

## 핵심 설계 원칙 정리

### 1. REVIEWER는 판정만, REFACTOR는 실행만

```
REVIEWER  → 코드를 읽고 개선 항목을 서술한다. 코드는 수정하지 않는다.
REFACTOR  → REVIEWER가 지적한 항목을 구현한다. 스스로 무엇을 할지 결정하지 않는다.
```

### 2. Phase PLAN은 자기완결형

각 Phase PLAN.md는 해당 사이클을 실행하는 에이전트가 다른 문서를 참조하지 않아도  
필요한 모든 정보(파일 경로, 전달 내용, 판정 기준, 커밋 명령)를 포함한다.

### 3. 재시도 상한

| 에이전트 | 재시도 상한 |
|----------|-------------|
| red-agent | 3회 |
| green-agent | 3회 |
| refactor-agent | 3회 (REFACTOR 후 회귀 시 2회) |
| reviewer-agent | 재시도 없음 |
| build-git-agent | 재시도 없음 (판정 후 위임) |

### 4. 커밋은 항상 사용자 승인 후

build-git-agent는 전체 사이클 완료 후 변경 요약을 제시하고  
사용자 승인을 받은 뒤에만 커밋·푸시를 실행한다.
