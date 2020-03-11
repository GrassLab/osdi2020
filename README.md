# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0510507`| `zhengwei` | `吳政緯` | wzhengwei6@gmail.com |

## How to build

make

## How to run
make run

## How to burn it into pi3


 - 下載 bootable image
 - 先查看 SD 卡掛在 dev 下的名字
  `df -h`
 - 反掛載 /dev/sdf，避免再運行過程中有其他的寫入
  `umount /dev/sdb1` 
 - 格式化 SD Card 成 FAT32
  `sudo umount /dev/sdb1`
  `sudo mkdosfs -F 32 -v /dev/sdb1`
 - 利用dd: 意為 data description， 能夠將輸入寫到標準輸出中
  `dd if=nctuos.img of=/dev/sdb`

## Architecture

## Directory structure


