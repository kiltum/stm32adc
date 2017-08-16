/* Here is adapted version HAL_ADC_ConfigChannel from STM32 HAL CODE 

  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.

*/

HAL_StatusTypeDef ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig)
{
  HAL_StatusTypeDef tmp_hal_status = HAL_OK;
  __IO uint32_t wait_loop_index = 0U;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_CHANNEL(sConfig->Channel));
  assert_param(IS_ADC_RANK(sConfig->Rank));
  
  if (! IS_ADC_SAMPLE_TIME(hadc->Init.SamplingTimeCommon))
  {
    assert_param(IS_ADC_SAMPLE_TIME(sConfig->SamplingTime));
  }
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated when ADC is disabled or enabled without   */
  /* conversion on going on regular group:                                    */
  /*  - Channel number                                                        */
  /*  - Channel sampling time                                                 */
  /*  - Management of internal measurement channels: VrefInt/TempSensor/Vbat  */
  if (ADC_IS_CONVERSION_ONGOING_REGULAR(hadc) == RESET)
  {
   
      hadc->Instance->CHSELR = ADC_CHSELR_CHANNEL(sConfig->Channel);
      
      /* Channel sampling time configuration */
      /* Management of parameters "SamplingTimeCommon" and "SamplingTime"     */
      /* (obsolete): sampling time set in this function with                  */
      /* parameter "SamplingTime" (obsolete) only if not already set into     */
      /* ADC initialization structure with parameter "SamplingTimeCommon".    */
      if (! IS_ADC_SAMPLE_TIME(hadc->Init.SamplingTimeCommon))
      {
        /* Modify sampling time if needed (not needed in case of reoccurrence */
        /* for several channels programmed consecutively into the sequencer)  */
        if (sConfig->SamplingTime != ADC_GET_SAMPLINGTIME(hadc))
        {
          /* Channel sampling time configuration */
          /* Clear the old sample time */
          hadc->Instance->SMPR &= ~(ADC_SMPR_SMP);
          
          /* Set the new sample time */
          hadc->Instance->SMPR |= ADC_SMPR_SET(sConfig->SamplingTime);
        }
      }
      
      /* Management of internal measurement channels: VrefInt/TempSensor/Vbat */
      /* internal measurement paths enable: If internal channel selected,     */
      /* enable dedicated internal buffers and path.                          */
      /* Note: these internal measurement paths can be disabled using         */
      /*       HAL_ADC_DeInit() or removing the channel from sequencer with   */
      /*       channel configuration parameter "Rank".                        */
      if(ADC_IS_CHANNEL_INTERNAL(sConfig->Channel))
      {
        /* If Channel_16 is selected, enable Temp. sensor measurement path. */
        /* If Channel_17 is selected, enable VREFINT measurement path. */
        /* If Channel_18 is selected, enable VBAT measurement path. */
        ADC->CCR |= ADC_CHANNEL_INTERNAL_PATH(sConfig->Channel);
        
        /* If Temp. sensor is selected, wait for stabilization delay */
        if (sConfig->Channel == ADC_CHANNEL_TEMPSENSOR)
        {
          /* Delay for temperature sensor stabilization time */
          /* Compute number of CPU cycles to wait for */
          wait_loop_index = ( 10 * (SystemCoreClock / 1000000U));
          while(wait_loop_index != 0U)
          {
            wait_loop_index--;
          }
        }
      }
    
	
    
  }
   
  /* If a conversion is on going on regular group, no update on regular       */
  /* channel could be done on neither of the channel configuration structure  */
  /* parameters.                                                              */
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
    
    tmp_hal_status = HAL_ERROR;
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_hal_status;
}



int GetADCValue(uint32_t Channel)
{
	int val = 0;
	int Count = 3; 
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel=Channel;
	sConfig.Rank=ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;
	ADC_ConfigChannel(&hadc,&sConfig);
	
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc,100000);
	
	for(int i = 0; i < Count; i++)
	{		
		val += HAL_ADC_GetValue(&hadc);
	}
return val / Count;
}

