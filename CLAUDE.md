# CLAUDE.md — BowlingTDD

## 프로젝트 개요

Visual Studio 2022 + GMock(gtest) 환경에서 볼링 점수 계산기를 TDD로 구현하는 실습 프로젝트.

## 빌드

```powershell
# MSBuild (x64 Release)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" BowlingTDD.sln /p:Configuration=Release /p:Platform=x64

# 실행
.\x64\Release\BowlingTDD.exe
```

## 테스트

- 테스트 진입점: `BowlingTDD/main.cpp`
- gtest/gmock은 NuGet 패키지 `gmock 1.11.0`으로 설치되어 있다 (`packages/gmock.1.11.0/`)
- 빌드 후 실행 파일을 돌려 테스트 결과를 확인한다

## Git Commit 규칙

커밋 메시지를 작성하기 전에 반드시 **[`doc/COMMIT_CONVENTION.md`](doc/COMMIT_CONVENTION.md)** 를 참고한다.

- Type / Scope / Subject / Body / Footer 형식을 따른다
- TDD 흐름(Red → Green → Refactor)이 드러나도록 `test`, `feat`, `refactor` type을 적극 활용한다
