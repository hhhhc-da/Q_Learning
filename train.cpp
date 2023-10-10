
#include "train.h"

// 全局变量设置
namespace
{
    // RewardMatrix: R表，大小 6*6
    // ValueMatrix : V表，大小 6*6
    Eigen::Matrix<map_type, 6, 6> RewardMatrix, ValueMatrix;

    // 学习率、探索率、衰减率
    double learningRate = 1, exploreRate = 0.1, gamaRate = 0.8;
    // 训练次数
    unsigned epoch = 200;
    // 游戏运行标识
    status stat = GAME_CTNE;

    // 全局随机数引擎
    std::default_random_engine randomEngine;
    std::uniform_real_distribution<double> randomExample(0, 1);
    std::uniform_int_distribution<unsigned> randomRoom(0, 4);
}

int main()
{
    /* 封闭的道路使用负数，负数不作为随机方向 */
    // 初始化奖励矩阵
    RewardMatrix << -1, -1, -1, 0, -1, -1,
        -1, -1, 0, 0, 0, -1,
        -1, 0, -1, -1, -1, 100,
        0, 0, -1, -1, -1, -1,
        -1, 0, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1;

    // 初始化价值表
    ValueMatrix << 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0;

    /* 合理赋值，引导快速收敛 */
    // 教师强制价值表
    // ValueMatrix << 0, 0, 0, 10, 0, 0,
    //     0, 0, 20, 0, 0, 0,
    //     0, 0, 0, 0, 0, 100,
    //     0, 10, 0, 0, 0, 0,
    //     0, 10, 0, 0, 0, 0,
    //     0, 0, 0, 0, 0, 0;

    Environment env;

    while (epoch--)
    {
        env.setEnv(randomRoom(randomEngine));
        std::cout << "Epoch: " << 200 - epoch << std::endl;

        env.setAction(std::make_pair(env.getContentment(), env.getContentment()));
        env.setValue(0);

        while (true)
        {
            col_type vec;

            if (randomExample(randomEngine) < exploreRate)
            {
                // 探索模式
                vec = GetRewardVector(env, -1);
#if PRINT
                std::cout << "探索模式" << std::endl;
#endif
            }
            else
            {
                // 经验模式
                vec = GetValueVector(env, -1);
#if PRINT
                std::cout << "经验模式" << std::endl;
#endif
            }

            act_type act = JudgeAction(env, vec);

            status st = TakeAction(env, act);

            // 校验错误
            if (stat != st)
                std::cout << "Proceecing Error: Stat != st" << std::endl;

            if (stat == GAME_OVER)
            {
                break;
            }
        }

        std::cout << "--------------------------------" << std::endl;
        PrintValueMatrix();
        std::cout << "--------------------------------" << std::endl;

        // 重置游戏状态
        stat = GAME_CTNE;
    }

    system("pause");
    return 0;
}

map_type ValueCalculate(Environment &env, act_type act, status &st)
{
    // 如果游戏已经结束，直接返回 0.0
    if (st == GAME_OVER)
    {
        return 0.0;
    }

    env_type roomIndex = env.getContentment();
    map_type value = env.getValue();
    act_type recentAction = act;

    // 校验行为起点和当前环境是否匹配
    assert(roomIndex == recentAction.first && "Please input correct input");

    // max( V(S',A')
    auto col_next = GetValueVector(env, act.second);

    // 这里不可以直接 decltype(col_next(0)) 会得到引用
    map_type max_predict = col_next[0];

    for (std::size_t i = 0; i < col_next.size(); ++i)
    {
        if (col_next[i] > max_predict)
        {
            max_predict = col_next[i];
        }
    }

    // 游戏结束的情况
    if (act.second == 5)
    {
        max_predict = 100;
    }

    // Q_Learning: V(S,A) ← V(S,A) + α( γ( max( V(S',A') ) ) - V(S,A) )
    map_type newValue = value + learningRate * (RewardMatrix(act.first, act.second) + gamaRate * max_predict - value);

    // 更新 Value 表格
    ValueMatrix(recentAction.first, recentAction.second) = newValue;

#if PRINT
    std::cout << "赋值: V(" << recentAction.first << "," << recentAction.second << ") 为" << newValue << std::endl;
    // std::cout << "获取 Value:" << newValue << std::endl;
#endif

    return newValue;
}

status TakeAction(Environment &env, act_type act)
{
    // 检查当前环境和行为起点是否匹配
    assert(act.first == env.getContentment() && "Please input correct room index.");

    // 防止左右值问题
    unsigned t_right = act.second;

    auto v = ValueCalculate(env, act, stat);

    env.setValue(v);
    env.setAction(act);

    // 更新当前环境
    env.setEnv(t_right);

    if (t_right == 5)
    {
        // 游戏结束
        stat = GAME_OVER;

        // std::cout << "游戏结束" << std::endl;

        return GAME_OVER;
    }

    return GAME_CTNE;
}

col_type GetRewardVector(Environment &env, int iline)
{
    // 获取现在在哪一行
    env_type roomIndex = env.getContentment();

    auto col = RewardMatrix.col(roomIndex);
    auto line = env.getContentment();

    if (iline != -1)
    {
        line = iline;
    }

    std::vector<map_type> vec;

    for (int i = 0; i < col.size(); ++i)
    {
        vec.push_back(RewardMatrix(line, i));
    }

    return vec;
}

col_type GetValueVector(Environment &env, int iline)
{
    // 获取现在在哪一行
    env_type roomIndex = env.getContentment();

    auto col = RewardMatrix.col(roomIndex);
    auto line = env.getContentment();

    if (iline != -1)
    {
        line = iline;
    }

    std::vector<map_type> vec;

    for (int i = 0; i < col.size(); ++i)
    {
        vec.push_back(ValueMatrix(line, i));
    }

    return vec;
}

act_type JudgeAction(Environment &env, col_type &jgt)
{
    std::set<map_type> rst;
    std::vector<map_type> index;
    map_type max = 0;

    rst.clear();

    for (std::size_t i = 0; i < jgt.size(); ++i)
    {
        // 检索本行所有的 Value/Reward 值
        auto in = jgt[i];
        rst.insert(in);
    }

    // 取出来最大的
    for (auto s : rst)
    {
        max = s;
    }

    // 排除为负数的情况
    for (std::size_t i = 0; i < jgt.size(); ++i)
    {
        if (jgt[i] == max && RewardMatrix(env.getContentment(), i) >= 0)
        {
            // 把最大的 index 放到 vector 里
            index.push_back(i);
        }
    }

    act_type action;

    // 返回值赋值
    action.first = env.getContentment();

    // 未找到索引
    if (index.size() == 0)
    {
        std::cout << "决策错误 - 0" << std::endl;
    }

    // 如果只有一个最大值，那么不需要随机选择
    if (index.size() == 1)
    {
        // 返回值赋值
        action.second = index.at(0);
    }
    else
    {
        // 有多个直接随机抽一个选
        auto count = index.size();

        std::uniform_int_distribution<unsigned> randomExample(0, count - 1);

        auto ran = randomExample(randomEngine);
        // 返回值赋值
        action.second = index.at(ran);
    }

    return action;
}

void PrintValueMatrix(void)
{
    std::cout << "Value matrix:" << std::endl;

    for (int i = 0; i < 6; ++i)
    {
        std::cout << "[ ";
        for (int j = 0; j < 6; ++j)
        {
            std::cout << ValueMatrix(i, j) << " ";
        }
        std::cout << "]\n";
    }
}