﻿/*
 * 작성자: 윤정도
 * 생성일: 2/4/2023 9:36:50 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "Player.h"

Player::Player()
	: Character()
{}

Player::~Player()
{}

bool Player::init() {
	initPlayerData();
	return true;
}

void Player::initPlayerData() {
	m_PlayerData.clear();
}