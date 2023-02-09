/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:06:04 PM
 * =====================
 *
 */


#pragma once
#pragma warning(disable: 26482) // Only index into arrays using constant expressions.


// enum class 쓰긴 싫고 enum은 타입 이름이 겹쳐서 싫고 대안으로 struct안에 enum 끼여서 쓰는건 어떨까?
// 이번에 개발 할때 써보고 영 아니면 폐기하는걸로, 쓴지 하루지났는데 아직 갠찮은듯
#define SEnumBegin(enum_name) struct enum_name { enum_name() = delete; enum _ {
#define SEnumInteritedBegin(enum_name, base_enum) struct enum_name : base_enum { enum_name() = delete; enum _ {
#define SEnumMiddle(enum_name) }; using enum_name##_t = enum_name::_;
#define SEnumMiddleEnd(enum_name) }; using enum_name##_t = enum_name::_;
#define SEnumEnd(enum_name) }; }; using enum_name##_t = enum_name::_;


SEnumBegin(CharacterType)
Gunner,
Max
SEnumEnd(CharacterType)

SEnumBegin(SpriteDirection)
Right	,
Left	,
Max
SEnumEnd(SpriteDirection)


SEnumInteritedBegin(Direction, SpriteDirection)
Up = SpriteDirection::Max,
Down	,
Max		,
None
SEnumEnd(Direction)



SEnumBegin(JumpDirection)
None,
Up = Direction::Up,
Down
SEnumEnd(JumpDirection)


SEnumBegin(AvatarPartType)
Begin		,
Skin = Begin,
Shoes		,
Pants		,
Neck		,
Hair		,
Face		,
Coat		,
Cap			,
Belt		,
End = Belt	,
Max
SEnumEnd(AvatarPartType)


SEnumInteritedBegin(CharacterPartType, AvatarPartType)
Begin		,
Skin = Begin,
Shoes		,
Pants		,
Neck		,
Hair		,
Face		,
Coat		,
Cap			,
Belt		,
Weapon		,
End = Weapon,
Max
SEnumEnd(CharacterPartType)
