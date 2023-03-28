#!/usr/bin/python
# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
import math
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm
import seaborn as sns

start = [253,337]
goal = [55,175]

#微分と進むスピード
delt  = 0.1
speed = 10

#障害物とゴールの重みづけ
weight_obst, weight_goal = 100, 0.1

#それぞれの軸の範囲
x_min, y_min = 0, 0
x_max, y_max = 520, 370

#ポテンシャルの最大値、最小値
potential_max, potential_min = 1, -1

#障害物と認識する距離
minimum_dist = 10
dists = np.loadtxt("distcache.csv", delimiter=',')

sns.set()

#ポテンシャル関数の計算
def cal_pot(x, y):
  print(dists.shape, y,x)
  obst_pot =  weight_obst / dists[int(y)][int(x)]

  #ゴールの座標はpotentialはmin
  if goal[0] == x and goal[1] == y:
    goal_pot = potential_min
  else:
    goal_pot = -1 / math.sqrt(pow((x - goal[0]),  2) + pow((y - goal[1]),  2))
  pot_all    = obst_pot + weight_goal * goal_pot
  return pot_all

#ルートをdfに代入
def cal_route(x, y, df):
  count = 0
  while True:
    count += 1
    if x < 0 or y < 0 or x > x_max or y > y_max :
        print("End!!")
        return df
    vx = -(cal_pot(x + delt, y) - cal_pot(x, y)) / delt
    vy = -(cal_pot(x, y+delt) - cal_pot(x, y)) / delt

    v = math.sqrt(vx * vx + vy * vy)

    # 正規化
    vx /= v / speed
    vy /= v / speed

    # 進める
    x += vx
    y += vy

    # Series型でdfに追加
    tmp = pd.Series([x, y, vx, vy], index = df.columns)
    df = df.append(tmp, ignore_index = True) 
    print(len(df))

    # ゴールに近づいた場合，10,000回ループした場合，終了
    if (goal[0] - x)**2 + (goal[1] - y)**2 < 3:
      print("Goal ")
      break
    if count > 10000:
      print("maxiter!!")
      break
  return df

#ルートグラフ化
def plot_route(df):
  plt.scatter(df['x'],df['y'])
  #スタート、ゴール、障害物をプロット
  plt.plot(start[0]  , start[1]  , marker = 's', color = 'b', markersize = 15)
  plt.plot(goal[0]   , goal[1]   , marker = 's', color = 'b', markersize = 15)

  plt.imshow(dists, cmap='jet', interpolation='nearest')
  plt.xlim([x_min, x_max])
  plt.ylim([y_min, y_max])
  plt.show()

#全体のポテンシャル場の計算
def cal_potential_field():
  pot = []
  for y_for_pot in range(y_min, y_max + 1):
    tmp_pot = []
    for x_for_pot in range(x_min, x_max + 1):
      potential = 0
      potential += cal_pot(x_for_pot, y_for_pot)
      #max,minの範囲内にする
      if potential > potential_max:
        potential = potential_max
      elif potential < potential_min:
        potential = potential_min

      tmp_pot.append(potential)
    pot.append(tmp_pot)

  pot = np.array(pot)
  return pot
#ポテンシャル場グラフ化
def plot3d(U,xm,ym):
    # グラフ表示の設定
    plt.figure(figsize=(6,4))
    fig = plt.figure(facecolor="w")
    ax = fig.add_subplot(111, projection="3d")
    ax.tick_params(labelsize=7)    # 軸のフォントサイズ
    ax.set_xlabel("x", fontsize=10)
    ax.set_ylabel("y", fontsize=10)
    ax.set_zlabel("U", fontsize=10)
#     surf = ax.plot_surface(xm, ym, U, rstride=1, cstride=1,linewidth=1, antialiased=True, cmap='bwr')
    surf = ax.plot_surface(xm, ym, U, rstride=1, cstride=1, cmap=cm.coolwarm)
    plt.show()

def main():
  pot = cal_potential_field()
  # x_plot, y_plot = np.meshgrid(np.arange(x_min, x_max + 1),np.arange(y_min, y_max +1))
  # plot3d(pot, x_plot, y_plot)

  df = pd.DataFrame(columns=['x','y','vx','vy'])
  df = cal_route(start[0], start[1], df)
  plot_route(df)


if __name__ == '__main__':
    print("Start rrt start planning")
    main()
