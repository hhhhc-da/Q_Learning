# Q_Learning
A fundmental C++ Q_Learning reinforcement learning project.

给出6个房间，其中有如下关系（两个房间之间互相连通记为<x,y>）:

```
<0,3>, <1,2>, <1,3>, <1,4>, <2,5>
```

tips: 房间是双向连通的，要操纵机器人前往房间 5

项目使用 Q_Learning 算法实现的训练过程：

```
V(S,A) ← V(S,A) + α * (R + γ * V(S',A') - V(S,A))
```
