# Note

<!-- TOC -->
* [Note](#note)
  * [Reflection and GC](#reflection-and-gc)
  * [Delta Time](#delta-time)
    * [P29 Blending Shooting Animations](#p29-blending-shooting-animations)
<!-- TOC -->



## Reflection and GC



## Delta Time

DeltaTime: 相邻两帧的时间差

Frame：更新到屏幕上的图片

Frame Rate：每秒更新的帧数FPS

Tick：等同于Frame帧

$$
speed(\frac{cm}{second}) \times DeltaTime(\frac{second}{frame}) = \frac{cm}{frame}
$$


### P29 Blending Shooting Animations

为了实现在播放WeaponFire动画时，下半身能够正常播放移动动画，使用 `Layered blend per bond`

<img alt="image-20231115141230567" src="C:\Users\xyz\AppData\Roaming\Typora\typora-user-images\image-20231115141230567.png"/>



P55 Apply Additive

