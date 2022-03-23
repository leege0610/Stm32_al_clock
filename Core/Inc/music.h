/*
 * music.h
 *
 *  Created on: Jan 20, 2022
 *      Author: LEE
 */
#ifndef INC_MUSIC_H_
#define INC_MUSIC_H_

#define C	262
#define D	294
#define E	330
#define F	349
#define G	392
#define A	440
#define B	494
#define C1	523
#define D1	587
#define E1	659

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

typedef struct {
  int8_t music_num;
  char music_title[17];
}MusicTypeDef;

typedef struct{
	uint16_t freq;
	uint16_t delay;
}_BUZZER;

#endif /* INC_MUSIC_H_ */
