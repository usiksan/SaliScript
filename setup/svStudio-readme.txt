В /etc/udev/rules.d разместить файл usb.conf
sudo copy usb.conf /etc/udev/rules.d

Установить библиотеку usb-1.0 и usb-0.1-dev
sudo apt-get install libusb-1.0*
sudo apt-get install libusb-dev
