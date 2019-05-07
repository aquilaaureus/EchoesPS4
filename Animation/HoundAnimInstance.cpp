// Fill out your copyright notice in the Description page of Project Settings.

#include "HoundAnimInstance.h"


void UHoundAnimInstance::PlayIdleAnimation()
{
	if (!m_bDeathTrigger)
	{
		m_bIdleTrigger = true;
		m_bAttackTrigger = false;
		m_bGrabTrigger = false;
	}
}

void UHoundAnimInstance::PlayAttackAnimation()
{
	if (!m_bDeathTrigger)
	{
		m_bIdleTrigger = false;
		m_bGrabTrigger = false;
		m_bAttackTrigger = true;
	}
}

void UHoundAnimInstance::PlayGrabbingAnimation()
{
	if (!m_bDeathTrigger)
	{
		m_bIdleTrigger = false;
		m_bAttackTrigger = false;
		m_bGrabTrigger = true;
	}
}

void UHoundAnimInstance::PlayDeathAnimation()
{
	m_bDeathTrigger = true;
	m_bIdleTrigger = false;
	m_bAttackTrigger = false;
	m_bGrabTrigger = false;
}
