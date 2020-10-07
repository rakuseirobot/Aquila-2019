# Aquila-2019

Robocup Junior Rescue Maze Program

このリポジトリは2019年度のAquilaのメインプログラムです。

# Note

レスキューメイズのロボットを動かすためのSTM32F446用のプログラムです。

二輪で4つのタイヤを動かして動かします。

前左右にOpenMVというカメラモジュールを搭載し、[H,S,U]の文字を認識しながらロボットは進みます。

左右に2個ずつ、前後にひとつずつ超音波センサーを使っています。

ブザーやLCDなどの動作プログラムなども含まれています。

モーター制御にはPID制御を使用しています。eepromも使用することができます。

ですが、開発していたそのままなので、全体的に見にくいコードになって居て申し訳ないです。。。

# Link

[About robot](https://qiita.com/Shunk_/items/b6b5c49862ec9f9d852b)

[How to make RCJM](https://qiita.com/Shunk_/items/2ea0795d571d771d52ca)

[回路](https://github.com/rakuseirobot/Aquila-PCB)

[STM32-program](https://github.com/rakuseirobot/Aquila-2019)

[OpenMV](https://github.com/rakuseirobot/Aquila-OpenMV)

[設計図](https://github.com/rakuseirobot/Aquila-drawings)

[古いメインプログラム(ATxmega)](https://github.com/rakuseirobot/Aquila-Code-v3)

# Author

* Shun Kayaki

    Kyushu institute of Technology
    
    shun@guetan.dev

* Emile

# License

Copyright (c) 2019 Aquila
Released under the MIT license
