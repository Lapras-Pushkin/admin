#pragma once
#ifndef STAFF_H
#define STAFF_H

// 声明医护端对外的接口函数
void prescribeMedicine(const char* currentDoctorId, const char* currentPatientId);
void staffTerminal();

#endif