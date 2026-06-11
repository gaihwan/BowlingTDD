# Commit Convention

## 형식

```
<type>(<scope>): <subject>

[body]

[footer]
```

## Type

| Type       | 설명 |
|------------|------|
| `feat`     | 새로운 기능 추가 |
| `fix`      | 버그 수정 |
| `test`     | 테스트 코드 추가·수정 (프로덕션 코드 변경 없음) |
| `refactor` | 기능 변경 없는 코드 구조 개선 |
| `docs`     | 문서 작성·수정 |
| `chore`    | 빌드 설정, 패키지, 기타 잡무 |
| `style`    | 코드 포맷·세미콜론 등 스타일만 변경 |

## Scope (선택)

변경 대상 모듈이나 파일명을 소문자로 작성한다.  
예: `bowling`, `game`, `frame`, `vcxproj`

## Subject

- 명령형 현재 시제로 작성한다 (Add, Fix, Remove …)
- 첫 글자는 대문자
- 마침표로 끝내지 않는다
- 50자 이내

## Body (선택)

- 72자 줄바꿈
- **무엇을** 했는지보다 **왜** 했는지를 설명한다

## Footer (선택)

- `Closes #이슈번호` 형식으로 이슈를 참조한다
- Breaking change는 `BREAKING CHANGE:` 접두사를 붙인다

---

## 예시

```
feat(game): Add BowlingGame class skeleton

Introduce BowlingGame as entry point for TDD implementation.
All methods are stubs to be filled in by upcoming tests.

Closes #1
```

```
test(game): Add failing test for gutter game

ASSERT_EQ(0, game.score()) fails as expected (Red phase).
```

```
fix(frame): Correct spare bonus calculation

Was applying bonus to wrong subsequent roll index.
```
