/***************************************************************************
					effectpreset.cpp  -  description
							 -------------------
		begin                : Sun Jun 17 2001
		copyright            : (C) 2001 by Jozef Kosoru
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include "control.h"
#include <cassert>
#include <iostream>
#include <qdatastream.h>
#include <klocale.h>
#include "soundprocessor.h"
#include "crchainview.h"
#include "crdatastream.h"
#include "creffectgui.h"
#include "threadeffector.h"
#include "effectkeeper.h"
#include "effectpreset.h"

#define EFFECT_PRESET_START       (0x51a351a3)
#define EFFECT_PRESET_UNIT_START  (0x31b331b3)

EffectPreset::EffectPreset(const EffectPresetSaveRequest& saveRequest, const EffectKeeper* effectKeeper)
	: m_presetName(saveRequest.m_presetName), m_saveChain(saveRequest.m_saveChain)
{
	assert(saveRequest.m_processorIdList.size() > 0);
	m_presetUnitList.reserve(saveRequest.m_processorIdList.size());

	ThreadEffector* const threadEffector = effectKeeper->threadEffector();
	const int chainSize = threadEffector->getProcessorChainSize();

	for(int count=0; count<chainSize; count++){
		for(std::vector<int>::const_iterator idIterator =  saveRequest.m_processorIdList.begin();
				idIterator != saveRequest.m_processorIdList.end(); idIterator++){
			if((*threadEffector)[count]->getId() == *idIterator){
				m_presetUnitList.push_back( new EffectPresetUnit((*threadEffector)[count]) );
				break;
			}
		}
	}
	assert(saveRequest.m_processorIdList.size() == m_presetUnitList.size());
}

EffectPreset::EffectPreset(QDataStream& dataStream) throw(Cr::CrException_presetDataError,std::bad_alloc)
{
	quint32 magic;
	dataStream >> magic;
	if(magic != EFFECT_PRESET_START){
          //throw(Cr::CrException_presetDataError(i18n("Error loading preset!")));
	}
	qint8 tmpVar;
	qint32 chainSize;
	dataStream >> m_presetName >> tmpVar >> chainSize;
	m_saveChain = static_cast<bool>(tmpVar);

	m_presetUnitList.reserve(static_cast<std::vector<EffectPresetUnit*>::size_type>(chainSize));
	for(int count=0; count<static_cast<int>(chainSize); count++){
		m_presetUnitList.push_back( new EffectPresetUnit(dataStream) );
	}
	assert(static_cast<int>(chainSize) == static_cast<int>(m_presetUnitList.size()));
}

EffectPreset::~EffectPreset()
{
	for(std::vector<EffectPresetUnit*>::const_iterator unitIterator = m_presetUnitList.begin();
			unitIterator != m_presetUnitList.end(); unitIterator++){
		//const int i = (*unitIterator)->processorId();
		delete *unitIterator;
	}
#ifdef _DEBUG_EFFECTPRESET
	std::cerr << "EffectPreset: " << m_presetName.latin1() << " deleted...\n";
#endif
}

void EffectPreset::loadEffect(EffectKeeper* effectKeeper)
{
	ThreadEffector* const threadEffector = effectKeeper->threadEffector();
	const int chainSize = threadEffector->getProcessorChainSize();

	std::vector<SoundProcessor*> activeProcessors;
	std::vector<SoundProcessor*> disabledProcessors;

	//make a list of the pointers to the active processors
	activeProcessors.reserve(m_presetUnitList.size());
	for(std::vector<EffectPresetUnit*>::const_iterator unitIterator = m_presetUnitList.begin();
			unitIterator != m_presetUnitList.end(); unitIterator++){
		for(int count=0; count<chainSize; count++){
			if((*unitIterator)->processorId() == (*threadEffector)[count]->getId()){
				activeProcessors.push_back((*threadEffector)[count]);
				break;
			}
		}
	}
	assert(m_presetUnitList.size() == activeProcessors.size());

	if(m_saveChain){
		//make a list of the pointers to the disabled processors
		disabledProcessors.reserve(static_cast<std::vector<SoundProcessor*>::size_type>(chainSize) - m_presetUnitList.size());
		for(int count=0; count<chainSize; count++){
			bool active = false;
			for(std::vector<EffectPresetUnit*>::const_iterator unitIterator = m_presetUnitList.begin();
					unitIterator != m_presetUnitList.end(); unitIterator++){
				if((*unitIterator)->processorId() == (*threadEffector)[count]->getId()){
					active = true;
					break;
				}
			}
			if(!active){
				disabledProcessors.push_back((*threadEffector)[count]);
			}
		}
		assert((static_cast<std::vector<SoundProcessor*>::size_type>(chainSize) - m_presetUnitList.size()) == disabledProcessors.size());

		//reorder processors
		std::vector<SoundProcessor*>::const_iterator activeIterator = activeProcessors.begin();
		for(int count=0; count < static_cast<int>(activeProcessors.size()); count++ ){
			(*threadEffector)[count] = *activeIterator++;
		}
		assert(activeIterator == activeProcessors.end());

		std::vector<SoundProcessor*>::const_iterator disabledIterator = disabledProcessors.begin();
		assert(chainSize == static_cast<int>(activeProcessors.size() + disabledProcessors.size()));
		for(int count = static_cast<int>(activeProcessors.size()); count < chainSize; count++){
			(*threadEffector)[count] = *disabledIterator++;
		}
		assert(disabledIterator == disabledProcessors.end());
	}

	//load processors parameters
	{
		std::vector<SoundProcessor*>::const_iterator activeIterator = activeProcessors.begin();
		std::vector<EffectPresetUnit*>::const_iterator unitIterator = m_presetUnitList.begin();
		while(activeIterator != activeProcessors.end()){
			(*unitIterator++)->loadEffectUnit( *activeIterator++ );
		}
	}

	if(m_saveChain){
		//disable inactive processors
		for(std::vector<SoundProcessor*>::const_iterator disabledIterator = disabledProcessors.begin();
				disabledIterator != disabledProcessors.end(); disabledIterator++){
			(*disabledIterator)->setMode(SoundProcessor::disabled);
		}
	}

	//update threadEffector chain
	threadEffector->updateProcessorChain();

	//update processors parameters / enable active processors
	for(std::vector<SoundProcessor*>::const_iterator activeIterator = activeProcessors.begin();
			activeIterator != activeProcessors.end(); activeIterator++){
		(*activeIterator)->updateParameters();
		(*activeIterator)->setMode(SoundProcessor::enabled);
	}

	//synchronize chainView buttons
	effectKeeper->chainView()->synchronize();

	//synchronize an effect GUI
	for( QListIterator<CrEffectGui*> effectIterator(effectKeeper->effectList());
             effectIterator.hasNext();){
          for(std::vector<SoundProcessor*>::const_iterator activeIterator = activeProcessors.begin();
              activeIterator != activeProcessors.end(); activeIterator++){
            CrEffectGui* effect = effectIterator.next();
            if(effect->getProcessor()->getId() == (*activeIterator)->getId()){
              effect->synchronize();
              break;
            }
          }
	}

}

void EffectPreset::saveEffect(QDataStream& dataStream) const
{
	dataStream << static_cast<quint32>(EFFECT_PRESET_START)
						 << m_presetName
						 << static_cast<qint8>(m_saveChain)
						 << static_cast<qint32>(m_presetUnitList.size());

	for(std::vector<EffectPresetUnit*>::const_iterator unitIterator = m_presetUnitList.begin();
			unitIterator != m_presetUnitList.end(); unitIterator++){
		(*unitIterator)->saveEffectUnit(dataStream);
	}
}

/** EffectPresetUnit */
EffectPreset::EffectPresetUnit::EffectPresetUnit(const SoundProcessor* processor)
	: m_processorId ( processor->getId() ),
		m_processorVersion ( processor->getVersion() ),
		m_data ( new char[processor->getParametersSize()] ),
		m_size ( processor->getParametersSize() )
{
	CrInputDataStream stream(m_data, m_size);
	*processor >> stream;
	assert(stream.eof());
#ifdef _DEBUG
	std::cerr << "EffectPresetUnit: " << processor->getName() << " - saved!\n";
#endif
}

EffectPreset::EffectPresetUnit::EffectPresetUnit(QDataStream& dataStream) throw(Cr::CrException_presetDataError,std::bad_alloc)
	: m_processorId(0), m_processorVersion(0), m_data(0), m_size(0)
{
	quint32 magic;
	dataStream >> magic;
	if(magic != EFFECT_PRESET_UNIT_START){
          //throw(Cr::CrException_presetDataError(i18n("Error loading preset unit!")));
	}

	qint32 tmpVar;
	dataStream >> tmpVar;
	m_processorId = static_cast<int>(tmpVar);
	dataStream >> m_processorVersion >> tmpVar;
	m_size = static_cast<int>(tmpVar);
	m_data = new char[m_size];

	for(qint8* value = reinterpret_cast<qint8*>(m_data);
			value < reinterpret_cast<qint8*>(m_data+m_size); value++){
		dataStream >> (*value);
	}
}

EffectPreset::EffectPresetUnit::~EffectPresetUnit()
{
	delete[] m_data;
}

void EffectPreset::EffectPresetUnit::loadEffectUnit(SoundProcessor* processor)
{
	assert(m_processorId == processor->getId() && m_processorVersion == processor->getVersion());
	CrOutputDataStream stream(m_data, m_size);
	*processor << stream;
	assert(stream.eof());
}

void EffectPreset::EffectPresetUnit::saveEffectUnit(QDataStream& dataStream) const
{
	dataStream << static_cast<quint32>(EFFECT_PRESET_UNIT_START)
						 << static_cast<quint32>(m_processorId)
						 << m_processorVersion
						 << static_cast<quint32>(m_size);

	for(qint8* value = reinterpret_cast<qint8*>(m_data);
			value < reinterpret_cast<qint8*>(m_data+m_size); value++){
		dataStream << (*value);
	}

}


