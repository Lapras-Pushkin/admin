#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "admin.h"

// ==========================================
// 模块一：药品管理 (对应图3的左侧分支)
// ==========================================
void medicineRestock() {
    char searchKey[50];
    int addAmount;

    printf("\n--- 药品管理：入库补给 ---\n");
    printf("请输入需要入库的药品名称或编号: ");
    scanf("%s", searchKey);

    Medicine* med = medicineHead->next;
    Medicine* targetMed = NULL;

    // 遍历查找药品
    while (med != NULL) {
        if (strcmp(med->name, searchKey) == 0 || strcmp(med->id, searchKey) == 0) {
            targetMed = med;
            break;
        }
        med = med->next;
    }

    if (targetMed == NULL) {
        printf("【错误】系统中未找到该药品，请先录入新药品档案！\n");
        return;
    }

    printf("【当前信息】药品: %s | 当前库存: %d | 单价: %.2f\n",
        targetMed->name, targetMed->stock, targetMed->price);

    printf("请输入入库数量: ");
    if (scanf("%d", &addAmount) != 1 || addAmount <= 0) {
        while (getchar() != '\n'); // 清理错误输入
        printf("【拦截】输入非法，入库数量必须为大于0的整数！\n");
        return;
    }

    // 更新库存
    targetMed->stock += addAmount;
    printf("【入库成功】%s 已成功入库 %d 份。最新库存为: %d\n",
        targetMed->name, addAmount, targetMed->stock);
}

// ==========================================
// 模块二：统计报表中心 (对应图3的中间分支)
// ==========================================
void financialReport() {
    printf("\n==================================\n");
    printf("        全院营业额度统计报表        \n");
    printf("==================================\n");

    Record* rec = recordHead->next;
    double totalRevenue = 0.0;
    int paidCount = 0;
    int unpaidCount = 0;

    printf("%-15s %-15s %-25s %-10s\n", "流水号", "患者ID", "账单明细", "实收金额");
    printf("--------------------------------------------------------------\n");

    // 遍历所有记录，统计财务数据
    while (rec != NULL) {
        if (rec->isPaid == 1) {
            printf("%-15s %-15s %-25s %.2f元\n",
                rec->recordId, rec->patientId, rec->description, rec->cost);
            totalRevenue += rec->cost;
            paidCount++;
        }
        else {
            unpaidCount++; // 统计跑单/未缴费的记录
        }
        rec = rec->next;
    }

    printf("--------------------------------------------------------------\n");
    printf("【报表总结】\n");
    printf("已结算账单数: %d 笔\n", paidCount);
    printf("待结算账单数: %d 笔\n", unpaidCount);
    printf("全院总营业额: %.2f 元\n", totalRevenue);
    printf("==================================\n");
}

// ==========================================
// 管理端主菜单调度
// ==========================================
void adminTerminal() {
    int choice;
    while (1) {
        printf("\n--- 管理端 (后台管理系统) ---\n");
        printf(" 1. 药品入库管理\n");
        printf(" 2. 财务营业额报表\n");
        printf(" 0. 返回主菜单\n");
        printf("请选择操作: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("输入无效，请输入数字！\n");
            continue;
        }

        switch (choice) {
        case 1: medicineRestock(); break;
        case 2: financialReport(); break;
        case 0: return;
        default: printf("选项不存在，请重新输入！\n");
        }
    }
}