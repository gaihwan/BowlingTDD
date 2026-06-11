# TDD 사전 Claude 세팅 — BowlingTDD 프로젝트

이 문서는 BowlingTDD 프로젝트를 시작하면서 Claude Code로 진행한 사전 환경 세팅 작업 전체를 순서대로 기록한다.

---

## 1. 작업 디렉터리 확인

**사용자 입력**
```
pwd
```

**결과**
- 현재 작업 디렉터리: `D:\02_Study\02_ClaudeCode\reviewer\TAKA\BowlingTDD`
- Visual Studio 솔루션(`BowlingTDD.sln`)과 프로젝트 폴더(`BowlingTDD/`)가 이미 존재하는 상태 확인

---

## 2. Hello World 출력 코드 작성 및 MSBuild 빌드

**사용자 입력**
```
현재 폴더는 visual studio 환경으로 솔루션 환경이 만들어 져 있고,
Hello World! 출력하는 코드 작성해주고, msbuild로 빌드 가능하게 해줘.
```

**수행 내용**

1. `BowlingTDD/main.cpp` 생성
```cpp
#include <iostream>

int main() {
    std::cout << "Hello World!" << std::endl;
    return 0;
}
```

2. `BowlingTDD/BowlingTDD.vcxproj` 에 소스 파일 등록
```xml
<ItemGroup>
  <ClCompile Include="main.cpp" />
</ItemGroup>
```

3. MSBuild 빌드 및 실행 확인
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
    BowlingTDD.sln /p:Configuration=Release /p:Platform=x64 /v:minimal
.\x64\Release\BowlingTDD.exe
# 출력: Hello World!
```

---

## 3. gtest ASSERTION 실패 코드 작성 및 빌드

**사용자 입력**
```
현재 환경은 gtest를 할 수 있도록 gmock이 설치되어 있어.
간단하게 ASSERTION 발생하는 코드 작성해서 빌드 해줘.
```

**수행 내용**

`BowlingTDD/main.cpp` 를 gtest 진입점으로 교체
```cpp
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(SampleTest, AssertionFails) {
    ASSERT_EQ(1, 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
```

빌드 및 실행 결과
```
[ RUN      ] SampleTest.AssertionFails
main.cpp(5): error: Expected equality of these values:
  1
  2
[  FAILED  ] SampleTest.AssertionFails
```

**핵심 확인 사항**
- `gmock 1.11.0` NuGet 패키지(`packages/gmock.1.11.0/`)가 이미 설치되어 있음
- `gmock.targets` 가 vcxproj에 자동으로 include 경로와 `gtest-all.cc` / `gmock-all.cc` 컴파일을 처리함
- `#include <gtest/gtest.h>` / `#include <gmock/gmock.h>` 로 바로 사용 가능

---

## 4. Commit Convention 작성 및 CLAUDE.md 초안 생성

**사용자 입력**
```
doc/COMMIT_CONVENTION.md 파일 생성후, 여기에 Commit Convention 초안 작성해줘.
CLAUDE.md 파일에 Git Commit 시 @doc/COMMIT_CONVENTION.md 파일을 참고하도록
Rule 을 적어줘.
Commit 해줘 메시지는 적절히 적어줘 현재 연결된 git hub에 push 까지 해줘.
```

**생성 파일**

`doc/COMMIT_CONVENTION.md` — 커밋 메시지 형식 정의
```
<type>(<scope>): <subject>

[body]
[footer]
```

| Type | 설명 |
|------|------|
| `feat` | 새로운 기능 추가 |
| `fix` | 버그 수정 |
| `test` | 테스트 코드 추가·수정 |
| `refactor` | 기능 변경 없는 코드 구조 개선 |
| `docs` | 문서 작성·수정 |
| `chore` | 빌드 설정, 패키지, 기타 잡무 |
| `style` | 코드 포맷·스타일만 변경 |

`CLAUDE.md` 초안 생성 — 빌드 명령어, 테스트 방법, 커밋 규칙 참조 링크 포함

**커밋**
```
chore: Add Hello World entry point, gtest setup, commit convention, and CLAUDE.md
```

---

## 5. Bowling-KATA.md 작성

**사용자 입력**
```
(볼링 점수 계산 문제 설명 및 요구사항 전문 제공)
이 내용으로 Bowling-KATA.md 작성해줘.
```

**생성 파일:** `doc/Bowling-KATA.md`

주요 내용:
- 미국식 텐핀 볼링 점수 계산 문제 설명 (Kata 범위: 유효성 검사·프레임 수 확인 제외)
- 스페어·스트라이크·10번 프레임 규칙 표로 정리
- `Game` 클래스 인터페이스 정의

```cpp
class Game {
public:
    void roll(int pins);  // 투구 시마다 호출
    int  score();         // 게임 총점 반환
};
```

- 점수 계산 예시 5가지 (거터 게임 0점 → 퍼펙트 게임 300점)

---

## 6. CLAUDE.md 개선 (`/init` 명령 사용)

**사용자 입력**
```
/init @doc/Bowling-KATA.md
```

**수행 내용**

기존 CLAUDE.md를 분석하여 아래 항목을 보강:

| 추가/개선 항목 | 내용 |
|---------------|------|
| Kata 목표 | `Game::roll` / `Game::score` 인터페이스와 문서 링크 |
| 프로젝트 구조 핵심 | vcxproj 파일 등록 방법, gmock include 경로 |
| TDD 사이클 커밋 예시 | Red/Green/Refactor 단계별 커밋 메시지 패턴 |
| 권장 테스트 순서 표 | Kata 5단계 시나리오와 기댓값 |
| 커밋 규칙 링크 | `doc/COMMIT_CONVENTION.md` 참조 명시 |

**커밋**
```
docs: Add Bowling Kata spec, TDD skill, and update CLAUDE.md
```

---

## 7. TDD 스킬 파일 생성 및 CLAUDE.md 연결

**사용자 입력**
```
.claude/skills/SKILL.md을 다음 내용으로 만들어주고,
이 스킬을 CLAUDE.md에 연결해줘.
---
name: test-driven-development
description: 모든 기능 개발 또는 버그 수정 시 구현 코드를 작성하기 전에 사용
             (C++ + Visual Studio + GoogleTest / GoogleMock)
---
(TDD 가이드 전문)
```

**생성 파일:** `.claude/skills/SKILL.md`

스킬 주요 구성:
- **절대 법칙:** 실패하는 테스트 없이 프로덕션 코드를 작성하지 말 것
- **Red-Green-Refactor** 사이클 다이어그램 및 각 단계 상세 설명
- **RED 검증 명령어** (Visual Studio Test Explorer / gtest 필터)
- **좋은 테스트 기준** 표 및 GoogleTest 명명 규칙
- **흔한 합리화 반박** 12가지
- **위험 신호** 체크리스트
- **gmock 사용 지침** (외부 의존성 있을 때만)
- **검증 체크리스트** 8항목

**CLAUDE.md 연결**
```markdown
> **스킬:** 기능 개발 또는 버그 수정 전 반드시
> `.claude/skills/SKILL.md` (`test-driven-development`) 를 참고한다.
```

---

## 8. 스킬 파일 위치 재구성

**사용자 입력**
```
.claude/skills에 있는 SKILL.md를
.claude/skills/test-driven-development 아래로 옮겨주고
commit 및 push 다시 해줘.
```

**수행 내용**

```
.claude/skills/SKILL.md
    → .claude/skills/test-driven-development/SKILL.md
```

CLAUDE.md 링크 경로 업데이트:
```markdown
[`.claude/skills/test-driven-development/SKILL.md`]
```

**커밋**
```
chore: Move SKILL.md into test-driven-development subdirectory
```

---

## 최종 파일 구조

```
BowlingTDD/
├── .claude/
│   └── skills/
│       └── test-driven-development/
│           └── SKILL.md          ← TDD 스킬 가이드
├── BowlingTDD/
│   ├── BowlingTDD.vcxproj
│   ├── main.cpp                  ← gtest 진입점 (현재 AssertionFails 테스트)
│   └── packages.config
├── doc/
│   ├── Bowling-KATA.md           ← 문제 정의 및 Game 클래스 인터페이스
│   └── COMMIT_CONVENTION.md      ← 커밋 메시지 규칙
├── lecture_docs/
│   └── tdd 사전 claude 세팅.md   ← 이 문서
├── packages/
│   └── gmock.1.11.0/             ← NuGet 패키지 (gtest + gmock)
├── CLAUDE.md                     ← Claude Code 가이드
└── BowlingTDD.sln
```

---

## 세팅 완료 후 상태

- Visual Studio 2022 + MSBuild 빌드 환경 동작 확인
- gtest/gmock 동작 확인 (ASSERTION 실패 출력 확인)
- `CLAUDE.md` — 빌드 명령, 테스트 방법, TDD 스킬 링크, 커밋 규칙 링크 포함
- `doc/COMMIT_CONVENTION.md` — 커밋 메시지 형식 정의
- `doc/Bowling-KATA.md` — Kata 문제 정의 및 `Game` 클래스 인터페이스 정의
- `.claude/skills/test-driven-development/SKILL.md` — TDD 스킬 등록 완료
- GitHub `origin/master` 최신 상태 유지
