//いい名前が思いつかなかったので後で名前の変更が必要
//直線移動とか加速度移動とかするための変数格納用のプログラム

#define FX_VALUE_TYPE_BOOL   0x00
#define FX_VALUE_TYPE_INT    0x01
#define FX_VALUE_TYPE_DOUBLE 0x02


class FxValue{
private:
    char value_data_type; //格納されている変数のデータ型
    int duration; //フレーム数

FxValue(char value_data_type_, int duration, int num_mid){ 

};

~FxValue(){};

};