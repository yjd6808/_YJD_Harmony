# sg (SteinsGate Common) 개발 TODO

## 패킷 프로토콜 정의
- [ ] 이동 명령어 (C_GAME_MOVE, S_GAME_MOVE) - 위치, 방향, 속도
- [ ] 점프 명령어 (C_GAME_JUMP, S_GAME_JUMP)
- [ ] 공격 명령어 (C_GAME_ATTACK, S_GAME_ATTACK) - 스킬ID, 방향
- [ ] 피격 명령어 (S_GAME_HIT) - 대상ID, 데미지, 넉백
- [ ] 스킬 사용 명령어 (C_GAME_SKILL, S_GAME_SKILL)
- [ ] 아이템 획득 명령어 (S_GAME_ITEM_DROP, C_GAME_ITEM_PICKUP)
- [ ] 캐릭터 사망/부활 명령어 (S_GAME_DIE, S_GAME_REVIVE)
- [ ] 던전 입장/퇴장 명령어 (C_GAME_ENTER_DUNGEON, S_GAME_ENTER_DUNGEON)
- [ ] 몬스터 스폰/사망 명령어 (S_GAME_SPAWN_MONSTER, S_GAME_KILL_MONSTER)
- [ ] 락스탭 프레임 동기화 명령어 (C_GAME_LOCKSTEP_INPUT, S_GAME_LOCKSTEP_FRAME)
- [ ] 채팅 명령어 확장 (귓속말, 파티 채팅, 던전 채팅)
- [ ] 파티 명령어 (C_PARTY_INVITE, S_PARTY_RESULT, S_PARTY_MEMBER_UPDATE)

## 게임 데이터 구조
- [ ] `CharInfo` - 캐릭터 기본 정보 (레벨, HP/MP, 스탯)
- [ ] `CharStats` - 스탯 구조체 (STR, INT, VIT, AGI 등)
- [ ] `ItemInfo` - 아이템 정보 (ID, 수량, 강화도, 랜덤 옵션)
- [ ] `SkillInfo` - 스킬 정보 (레벨, 쿨다운 여부)
- [ ] `MonsterInfo` - 몬스터 정보 (HP, 이동속도, 공격력)
- [ ] `DungeonInfo` - 던전 정보 (맵ID, 난이도, 몬스터 목록)
- [ ] `PartyInfo` - 파티 정보 (멤버 목록, 리더)
- [ ] `DropItemInfo` - 드롭 아이템 정보 (위치, 아이템, 소유권)

## 게임 상수 정의
- [ ] 캐릭터 직업 열거형 (Gunner, Fighter, Mage, Priest, Thief 등)
- [ ] 아이템 등급 열거형 (Common, Magic, Rare, Epic, Legendary)
- [ ] 아이템 종류 열거형 (Weapon, Armor, Accessory, Consumable)
- [ ] 스킬 타입 열거형 (Active, Passive, Toggle)
- [ ] 던전 난이도 열거형 (Normal, Hard, Expert, Master)
- [ ] 서버 최대 수용 인원 상수
- [ ] 던전 최대 파티 인원 상수 (4인)
- [ ] 게임 프레임 레이트 상수 (30fps 락스탭)

## 리소스
- [ ] 데이터 테이블 로더 (스킬 desc, 아이템 desc, 몬스터 desc)
- [ ] 스킬 데이터 테이블 (JSON/바이너리)
- [ ] 아이템 데이터 테이블
- [ ] 몬스터 데이터 테이블
- [ ] 던전 맵 데이터 포맷 정의
