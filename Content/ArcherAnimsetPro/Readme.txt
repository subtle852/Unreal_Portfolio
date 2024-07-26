To use the Archer model:
Rclick on SK_Archer
Skeleton->Assign Skeleton and select UE4_Mannequin_Skeleton - you will get warning because Archer have few more bones for cloths, but click Ok and Yes.
Assign skeleton one more time - now without warnings and save UE4_Mannequin_Skeleton
Now you can put Archer on the scene and use animations

Bow should be attached to the left hand or WeaponSocket
Quiver should be attached to QuiverSocket
Arrow shoud be attach to Bow_Arrow_Slot on the Bow.

Inside Animations/Bow you can find animations for Bow. For almost all animations you have to use "Bow_Idle_01_Bow" or "Bow_Idle_Aim_01_Bow" because string and arms of the bow are not moving. But for hit reactions, death, shooting etc. you have to pick animations from this folder.

Inside package you can find few animations for shooting forward - for more angles you should combine shooting animation with AimOffsets


Riko,
rikoanim@gmail.com