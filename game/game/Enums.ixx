export module Enums;

export enum class ObjectType {
	nonPhysical,
	staticBlock,
	player,
	playerAttack,
	enemy,
	enemyAttack,
	levelFinish
};

export enum class CollisionSide
{
	left,
	right,
	top,
	bottom,
};