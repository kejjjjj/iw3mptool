# iw3mptool // a multipurpose tool for call of duty 4 multiplayer

<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/b93fab71-3c95-4d95-be43-f992f25bcc91" />


<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/ef51026a-e546-4827-8bcd-d297caa48c87" />


## Features
- collision drawing
- map exporting 
- entity drawing
- debug information
- [scripting](https://github.com/kejjjjj/iw3mptool/wiki)

## Notes
- Only for cod4 multiplayer patches 1.7 and cod4x 21.1

## Usage
- The .asi file goes to the "miles" folder located in the cod4 root directory
- All commands start with the following prefixes:
    - cm_ (clip map)
    - pm_ (player movement)
    - sec_ (security)
    - varjus_ ([scripting engine](https://varjus.org))
- cm_showCollisionFilter expects the name of a material (e.g. "clip") and using it without an argument will clear the render queue
- cm_showEntities expects the name of a classname (e.g. "trigger script") and using it without an argument will clear the render queue
- you can include multiple arguments on cm_showCollisionFilter and cm_showEntities by using a space-separated list, (e.g. "clip caulk mantle")
- using the "all" argument on cm_showCollisionFilter and cm_showEntities will select *ALL* relevant items
- brushes are highlighted with green
- terrain brushmodels are highlighted with purple
- terrain is highlighted with blue
- entities are highlighted with red

<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/5d30263b-75e1-44eb-b9fd-4b77c15e3222" />

