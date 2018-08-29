![](https://ws1.sinaimg.cn/large/006tNbRwgy1fuhnfdwpfgj3042018jre.jpg)

# ITE4项目介绍

在 [ITE-Project-Road-Map](https://github.com/ITE-Organization/ITE-Project-Road-Map) 文档中，我们对 ITE4 做了简单介绍。

本文档，对ITE4的具体实现，做一个更详细的介绍。


## ITE4 : supernova

ITE4 supernova , 超新星， 是 ITE1、ITE2 的延续。也是 “建门游戏” 的最终完结版。

ITE1、ITE2 小规模试验版本，让我们找到了不断完善这个游戏的种种思路。

ITE4，相比于ITE1、2。 有了以下的改变：

1. 提前公布开局时间，采用和 [ITE3](https://github.com/ITE-Organization/ite3) 一样的防机器人策略。让所有人有一个更公平的开局起跑线。

2. 优化数值。让游戏过程更加合理。不会陷入完成不了游戏的僵局。

3. 游戏的盘子大小，改成动态增长。每成功结束一局，盘子自动增长 10%。（而不是之前roadmap中提到的 一下子增加10倍）。

4. 加入了和 ITE3 一样的推荐人奖励机制，任何人，只要有渠道和流量，均可以fork前端代码，代理本游戏（即 在你fork的代码里，把推荐人设为你自己。这样，通过你的渠道买入的玩家，推荐人都是你。）有意代理的，请发邮件给我们。
 
5. 吸取来自忠实玩家的宝贵建议，引入 “游戏即挖矿” 的概念。 凡是买入智子，按买入额 1:1 赠送 **ITE coin** （下个小节介绍 **ITE coin**）。

6. 首次成功购买智子之后，读取 ITE1 的合约信息，按 ITE1中 个人账号的 **智子** 持有数量， 1:1 空投 **ITE coin**。  （ITE1 老玩家回馈 ）

7. 首次成功购买智子之后，读取 ITE2 的合约信息，按 ITE2中 个人账号的 **智子**  持有数量， 1:1 空投 **ITE coin**。 （ITE2 老玩家回馈）

8. 首次成功购买智子之后，读取 ITE3 的合约信息，按 ITE3中 个人账号的 **ITE token**  持有数量， 1:2 空投 **ITE coin**。 （ITE 持股人回馈）

9. 10% 手续费收入，通过 “[虫洞协议](https://github.com/ITE-Organization/ite-wormholes)” 流入 ITE2，回馈ITE2的忠实玩家。

10. 20% 手续费收入，通过 “[虫洞协议](https://github.com/ITE-Organization/ite-wormholes)” 流入 ITE3，增长生态基金池并拉高ITE token的价格。


ITE4，将会是个全程紧张刺激的 “去中心化交易盘”。 

每一局的结束和开始阶段，将会是一个非常耐人寻味的博弈点。包括但不限于以下几点：

* 可以买入 博取 “建造者大奖”
* 可以回收 博取 “毁灭者大奖”
* 即使买入 没有夺得 大奖，却能自动成为下一局的 先进场者
* 可以大量卖出，破坏别人的 “小算盘”


## ITE coin 和 ITE token 的本质区别

ITE3 中公开发售的ITE token，它本质上是可分红的股权，它的特点是不可在市场上流通，不可在账号之间转账。

ITE coin， 则是允许自由转账，可以在市场上流通的代币。

ITE coin 的用途，在于回馈新老玩家。前期会采用各种形式赠送和空投。

由于代币的价值，取决于 “通证经济” 模型的设计，前期虽然可以自由转账，自由流通，但我们不保证其市场价格。

今后，我们会设计 ITE coin的多种用途、包括但不限于：

1. 直接用于玩今后的游戏
2. 购买游戏道具
3. 平台合作费、广告费支付

当然，我们会在今后的 ITE 去中心化交易所 中，率先上ITE coin。

发币容易，但使其产生价值，需要长久的努力。

我们承认目前团队中，少个 “通证经济” 设计师，

所以前期，也只能通过 “游戏挖矿”的形式， 先把 ITE coin 送出去。

至于其今后的实际流通价值。

我们会努力去创造。

## ITE coin 相关信息

* 最大发行量：10亿
* 采用按需发行的方式发行： 比如先给 ITE4 发行1000万，用于给玩家挖矿。
* 首次仅发行1000万，仅用与给ITE4 游戏挖矿 （包括游戏中给 ITE1 ITE2 ITE3玩家的空投）。

代币相关信息可以通过以下命令查询：

```
cleos -u http://api.eosnewyork.io get currency balance itecointoken itesupernova ITECOIN
cleos -u http://api.eosnewyork.io get currency stats itecointoken ITECOIN

{
  "ITECOIN": {
    "supply": "10000000.0000 ITECOIN",
    "max_supply": "1000000000.0000 ITECOIN",
    "issuer": "itecointoken"
  }
}

```
 
 ITE coin 已经上线TP钱包资产列表，代币符号 ITECOIN，图标如下：
 
![](https://ws1.sinaimg.cn/large/006tNbRwgy1fuohqs3107j30dw0dwtfv.jpg)

## ITE4 的上线时间


北京时间， 8月30号 20:00:00 

时间戳： 1535630400

开局时间确定的情况下，有N秒百分百拒绝转账效果，以及1分钟左右随机拒绝转账效果。大家看运气买入。

本策略是用于防合约机器人快速入场，本策略由合约自动执行，详情请看  [ITE3](https://github.com/ITE-Organization/ite3)。

![](https://ws2.sinaimg.cn/large/0069RVTdgy1fuqmxgzfk4j31kw0u9qj4.jpg)

![](https://ws4.sinaimg.cn/large/0069RVTdgy1fuqmz7e0hcj30mw0yujwd.jpg)

## 数值公开 和 经济推演

第一局数值：

总智子： 500000

保证金： 5000 EOS

工程难度： 85%

毁灭难度： 20%

单次充能、回收限制(总量2%): 10000 智子 = 100 EOS （额度随着工程进度提高而提高）

EOS最大容量： 18000 EOS （没错，第一局18000 EOS进场就能结束，各位下手轻点）

智子初始价格： 0.01 EOS

智子最高价格： 0.21 EOS （最高价会随着毁灭进度下降）

终极大奖最大值： 1800 EOS （扣除终极大奖后，剩余奖金按智子持有数量平分）

第二局数值：

总智子： 550000

保证金： 5500 EOS

工程难度： 85%

毁灭难度： 20%

单次充能、回收限制(总量2%): 11000 智子 = 110 EOS （额度随着工程进度提高而提高）

EOS最大容量： 20000 EOS （下手轻点）

智子初始价格： 0.01 EOS

智子最高价格： 0.21 EOS （最高价会随着毁灭进度下降）

终极大奖最大值： 2000 EOS （扣除终极大奖后，剩余奖金按智子持有数量平分）

第三局数值：

总智子： 600000

保证金： 6000 EOS

工程难度： 85%

毁灭难度： 20%

单次充能、回收限制(总量2%): 12000 智子 = 120 EOS （额度随着工程进度提高而提高）

EOS最大容量： 22000 EOS （下手轻点）

智子初始价格： 0.01 EOS

智子最高价格： 0.21 EOS （最高价会随着毁灭进度下降）

终极大奖最大值： 2200 EOS（扣除终极大奖后，剩余奖金按智子持有数量平分）

。。。。更多数据，有兴趣可以自行推演，全部数据都在链上，获取命令是：

```
cleos get table itesupernova itesupernova global
cleos get table itesupernova itesupernova game

```

## 如何看待 ITE4

**ITE4: supernova**  可以视为 ITE 生态下第一个成熟的项目。 它有以下几个显著的特色：

* 第一个 ITE 生态下 通过 “[虫洞协议](https://github.com/ITE-Organization/ite-wormholes)” 不断向 [ITE3](https://github.com/ITE-Organization/ite3)  发起分红的项目
* 第一个 ITE 生态下 “游戏挖矿” 项目。 玩 ITE 4，即可获得 ITE coin。ITE coin 可以自由流通转账，已经上线TP钱包。未来将上交易所。

这是一个运行中涉及到6个以上智能合约交互运作的项目。在技术上，我们向更大的商业化DAPP迈向了一步。
ITE生态，未来可期。

## 后续 ITE 项目预告

* ITE dice: 做 **真.概率随机游戏** 。代码开源。数据全部在链上透明可查。买入和开奖同步执行。 这是个庄家也会输盘，目前我们还输不太起。所以定位是小赌怡情盘，最大买入额暂定 10 EOS。

* ITE dexchange:  bancor 协议正在引发一场革命。是发币、是ICO、是去中心化交易所、是种种你意想不到的运用。 ITE team 研究bancor已久，已经在多个项目中成功的运用bancor， **ITE dexchange** 将带来一个通用的 bancor 交易系统。 任何人，只要抵押EOS，即可上币。 从根本上解决小币种流通问题。我们将率先上 **ITE coin**， 也就是在 ITE3中玩游戏过程挖的币，将能够在 **ITE dexchange** 直接交易。 



