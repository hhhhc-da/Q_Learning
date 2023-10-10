#ifndef TRAIN_NANOKA_H
#define TRAIN_NANOKA_H

// 包含的头文件
#include "Eigen/Eigen/Eigen"
#include <iostream>
#include <string>
#include <utility>
#include <set>
#include <vector>
#include <random>
#include <assert.h>
#include <vector>

// 数据类型声明

// 行为类型
using act_type = std::pair<unsigned, unsigned>;
// 环境类型
using env_type = unsigned;
// 价值类型
using map_type = double;
// 游戏状态类型
using status = unsigned;
// 行类型
using col_type = std::vector<map_type>;

#define GAME_OVER 0
#define GAME_CTNE 1

// 行为类已被废弃

// 环境类
class Environment
{
public:
    // 默认初始化函数
    Environment() : roomIndex(0), reccentAction(std::make_pair(0, 0)), value(0)
    {
    }

    Environment(Environment &) = default;
    Environment(Environment &&) = default;
    ~Environment() = default;

    // 获取环境
    inline env_type getContentment(void)
    {
        return roomIndex;
    }

    inline act_type getRecentAction(void)
    {
        return reccentAction;
    }

    inline map_type getValue(void)
    {
        return value;
    }

    inline void setEnv(env_type ID)
    {
        roomIndex = ID;
    }

    inline void setValue(map_type v)
    {
        value = v;
    }

    inline void setAction(act_type a)
    {
        reccentAction.first = a.first;
        reccentAction.second = a.second;
    }

private:
    // 当前房间号
    env_type roomIndex;
    // 上次做的动作
    act_type reccentAction;
    // 当前拥有的 Value 值
    map_type value;
};

// 外部链接声明

// 计算当前点做一个动作的价值
map_type
ValueCalculate(Environment &env, act_type act, status &st);
// 开始执行行动并且更新 Value 表
status TakeAction(Environment &env, act_type act);
// 获取 Reward 的行向量
col_type GetRewardVector(Environment &env, int iline);
// 获取 Value 的行向量
col_type GetValueVector(Environment &env, int iline);
// 根据输入向量进行行为决策，返回最佳行为
act_type JudgeAction(Environment &env, col_type &jgt);
// 打印 Value 表
void PrintValueMatrix(void);

#endif