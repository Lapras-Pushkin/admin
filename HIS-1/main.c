#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "models.h"
#include "user.h"
#include "staff.h"
#include "admin.h"

// 全局链表头指针初始化
PatientList patientHead = NULL;
StaffList staffHead = NULL;
MedicineList medicineHead = NULL;
RecordList recordHead = NULL;
BedList bedHead = NULL;

// 初始化空链表 (带头结点)
void initLists() {
    patientHead = (PatientList)malloc(sizeof(Patient));
    patientHead->next = NULL;
    // 同理初始化其他链表...
}

void mainMenu() {
    int choice;
    while (1) {
        printf("\n==================================\n");
        printf("   小型医院轻量级医疗管理系统 (HIS)   \n");
        printf("==================================\n");
        printf("  1. 用户端入口 (患者服务)\n");
        printf("  2. 医护端入口 (医生/护士工作台)\n");
        printf("  3. 管理端入口 (后台管理)\n");
        printf("  0. 安全退出并保存数据\n");
        printf("==================================\n");
        printf("请输入您的选择: ");

        // 容错处理：灵活的数据输入方式（防止输入非数字导致死循环）
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // 清空输入缓冲区
            printf("【错误】输入无效，请输入数字！\n");
            continue;
        }

        switch (choice) {
        case 1: userTerminal(); break;
        case 2: staffTerminal(); break;
        case 3: adminTerminal(); break;
        case 0:
            printf("正在保存数据至本地TXT文件...\n");
            // saveAllDataToTxt(); // 这里调用文件保存函数
            printf("系统已安全退出，欢迎下次使用！\n");
            exit(0);
        default:
            printf("【错误】无效的选项，请重新输入！\n");
        }
    }
}

int main() {
    initLists();
    // loadAllDataFromTxt(); // 从本地txt读取前置测试数据 (至少30条)
    mainMenu();
    return 0;
}