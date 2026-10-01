[☁️ 云端 GitHub - 团队共享仓库]
  │
  ├── Stable_version (终极稳定版：CAN组网里程碑)
  ├── develop        (★公共集成干道：4人合并代码的终点，日常联调基准)
  │
  ├── dev_gmf        (你的远端分支：用于向 develop 发起 PR)
  ├── dev_lc      (队友的远端分支：用于向 develop 发起 PR)
  ├── dev_dxl      (队友的远端分支：用于向 develop 发起 PR)
  └── dev_ls      (队友的远端分支：用于向 develop 发起 PR)


[💻 你的电脑 - 负责 F407 主节点]
  │
  ├── develop        (只读/拉取：同步队友合进云端的最新代码)
  └── dev_gmf        (👉 工作台：基于最新 develop 切出，只 Push 到远端 dev_gmf)


[💻 队友B的电脑 - 负责 F103 从节点1]
  │
  ├── develop        (只读/拉取：同步队友合进云端的最新代码)
  └── dev_lc      (👉 工作台：基于最新 develop 切出，只 Push 到远端 dev_lc)


[💻 队友C的电脑 - 负责 F103 从节点2]
  │
  ├── develop        (只读/拉取：同步队友合进云端的最新代码)
  └── dev_dxl      (👉 工作台：基于最新 develop 切出，只 Push 到远端 dev_dxl)


[💻 队友D的电脑 - 负责 F103 从节点3]
  │
  ├── develop        (只读/拉取：同步队友合进云端的最新代码)
  └── dev_ls      (👉 工作台：基于最新 develop 切出，只 Push 到远端 dev_ls)

注意点：
1 个人本地仓库千万不能和develop分支合并他唯一作用就是拉取最新代码
2 个人分支dev_名字应推动到远端dev_名字分支上去
3 每个人代码推送到云端后点击合并代码由管理员审核看代码是否有冲突确认没冲突后，再合进 develop。
4 每个节点下的CMake文件是占位文件（由于git无法上传空文件），将工程保存在自己负责的目录下（比如gmf-->F407_Master）CubeMX生成的真实的 CMakeLists.txt 会直接覆盖掉现在的那个空占位文件
5 Shared_Inc 文件夹和 F407_Master、F103_Slave1 是平级的兄弟关系。当你在 F407_Master/Core/Src/main.c 里面写 #include "can_protocol.h" 时，编译器默认只会在 F407_Master 自己的目录里找，
肯定找不到。你需要通过修改 CMake 来告诉编译器去“隔壁”找。
解决办法：
打开 CubeMX 生成的那个真实的 CMakeLists.txt（比如在 F407_Master 目录下），向下滚动找到 target_include_directories 这个指令，把公共文件夹的相对路径 ../Shared_Inc 加进去：
# 这是 CubeMX 默认生成的包含路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # ...CubeMX自带的路径，比如 Core/Inc 等...
    
    # 👇 你需要手动追加这一行，.. 代表返回上一级目录
    ../Shared_Inc
)