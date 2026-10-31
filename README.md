\# UE5 第一人称射击游戏（FPS + 背包系统）



基于 Unreal Engine 5 + C++ 开发的第一人称射击 Demo，包含角色移动、射击、敌人 AI、血量 HUD 和背包系统。核心玩法逻辑全部使用 C++ 实现，蓝图仅负责 UI 布局与资源挂接。



!\[游戏截图](docs/screenshot.png)



\## 演示视频



演示视频正在上传，稍后补充链接。



\## 功能



\- \*\*角色控制\*\*：WASD 移动、鼠标视角、空格跳跃，基于 Enhanced Input 实现

\- \*\*射击系统\*\*：射线检测即时命中，射速限制、弹药管理、R 键换弹

\- \*\*命中特效\*\*：Niagara 粒子附着到被击中目标，随敌人移动

\- \*\*敌人 AI\*\*：AIController + NavMesh，枚举状态机驱动巡逻、追击、攻击、死亡

\- \*\*血量系统\*\*：玩家和敌人都能受伤、死亡，玩家血量归零后禁用输入和移动

\- \*\*HUD\*\*：准星、血量、弹药，UMG 蓝图绑定函数实时读取 C++ 数据

\- \*\*背包系统\*\*：组件化 `UInventoryComponent`，`TArray` 管理物品，委托驱动 UI 刷新

\- \*\*物品拾取\*\*：按 E 射线检测拾取，准星对准时显示交互提示

\- \*\*物品使用\*\*：点击医疗包回血、数量减一、列表实时刷新，数量为 0 时该行消失



\## 技术栈



| 项目 | 内容 |

| --- | --- |

| 引擎 | Unreal Engine 5.8 |

| 语言 | C++（核心逻辑）、Blueprint（UI、资源挂接） |

| 输入 | Enhanced Input |

| UI | UMG |

| 特效 | Niagara |

| AI | AIController、NavMesh |



\## 技术点



\### C++ 系统设计



\- 角色、武器、敌人、背包、可拾取物均为独立 C++ 类，职责分离

\- 武器逻辑封装在 `AWeapon`，角色只负责输入转发，便于扩展新武器

\- 背包设计为 `UInventoryComponent` 组件，可复用到 NPC 或其他角色



\### 敌人 AI 状态机



\- 枚举 `EEnemyState` 驱动巡逻、追击、攻击、死亡四种状态

\- 结合视线检测和距离判定，实现"发现 → 追击 → 攻击"闭环

\- 攻击范围和追击停止距离设缓冲区间，避免状态在边界反复切换



\### 背包数据结构



\- `FItemData`（USTRUCT）描述物品：ID、名称、类型、数量、图标

\- `TArray<FItemData>` 存储，支持同 ID 物品叠加

\- `BlueprintAssignable` 动态多播委托 `OnInventoryUpdated` 通知 UI 刷新



\### 输入与 UI 焦点处理



\- 打开背包切换 `FInputModeGameAndUI`，显示鼠标光标

\- Button 取消 `Is Focusable`，避免点击后抢走键盘焦点导致 Tab/WASD 失效

\- `Shoot()` 判断背包打开时拦截射击，避免点击 UI 空白处误触开枪



\### 第一人称视觉



\- 第一人称手臂使用专用 Skeletal Mesh + `Use Animation Asset` 播放持枪 Idle

\- Camera 挂载到手臂 `Head` Socket，层级 `Capsule → FirstPersonMesh → Camera`

\- Tick 中同步 `FirstPersonMesh` 的 Pitch，上下视角时手臂跟随

\- 枪模型作为 Skeletal Mesh Component 附加到手臂，匹配持枪姿势



\## 项目结构



Source/demo/

├── FPSCharacter.h/.cpp # 玩家角色：移动、射击、拾取、背包、血量

├── Weapon.h/.cpp # 武器：开火、弹药、射速、射线检测、特效

├── EnemyCharacter.h/.cpp # 敌人：状态机、巡逻、追击、攻击、死亡

├── InventoryComponent.h/.cpp # 背包组件：物品管理、增删查、委托

├── ItemData.h/.cpp # 物品结构体与枚举

├── PickupItem.h/.cpp # 可拾取物品 Actor

└── demo.Build.cs # 模块依赖





\## 运行方式



1\. 安装 \*\*Unreal Engine 5.8\*\*

2\. 克隆仓库

3\. 右键 `demo.uproject` → \*\*Generate Visual Studio project files\*\*

4\. 用 Visual Studio 打开 `demo.sln`，按 `Ctrl+Shift+B` 编译

5\. 双击 `demo.uproject` 打开编辑器

6\. 打开地图 `Content/Maps/FPSMapAI`

7\. 点击播放



\## 操作说明



| 按键 | 功能 |

| --- | --- |

| WASD | 移动 |

| 鼠标 | 视角 |

| 空格 | 跳跃 |

| 鼠标左键 | 射击（按住连发） |

| R | 换弹 |

| E | 拾取 |

| Tab | 打开/关闭背包 |



\## 第三方资源



本项目使用了以下第三方资源，版权归各自作者所有，仅用于本项目的学习与演示，\*\*不适用本仓库的代码许可\*\*：



\- \*\*`Content/XL\_FPSPack/`\*\*：第一人称手臂、武器模型与动画，版权归原素材作者所有。

\- \*\*`Content/Characters/Mannequins/`\*\*：敌人使用的 Manny 骨骼网格体、骨架、材质与动画序列，版权归 \*\*Epic Games\*\* 所有。

\- \*\*`Content/Characters/Anims/`\*\*：敌人动画蓝图与动画序列，版权归 \*\*Epic Games\*\* 所有。



如需在其他项目中使用上述资源，请遵守各自原作者的许可协议。



\## 许可



\- \*\*`Source/` 下的 C++ 代码\*\*：本人独立编写，可自由阅读与参考。

\- \*\*自制资产\*\*：`Content/Blueprints/`、`Content/Input/`、`Content/Maps/` 下的蓝图、输入映射、关卡，`Content/Characters/Mannequins/Materials/M\_PickupGlow.uasset`（医疗包发光材质），以及 `Content/NS\_HitEffect.uasset`（基于 Niagara 模板调整的命中特效），版权归本人所有。

\- \*\*第三方资源\*\*（`Content/XL\_FPSPack/`、`Content/Characters/Mannequins/` 和 `Content/Characters/Anims/` 下的 Epic Games 资产等）\*\*不在上述许可范围内\*\*，版权归各自原作者所有。



\## 已知限制



\- 敌人使用枚举状态机，未使用行为树，适合小规模 AI

\- 未单独实现对象池，命中特效依赖 Niagara 自身生命周期管理

\- 单机 Demo，未实现网络同步

