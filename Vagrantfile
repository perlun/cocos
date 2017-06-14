# frozen_string_literal: true
Vagrant.configure(2) do |config|
  config.vm.box = 'remram/debian-9-i386'

  config.vm.provision 'shell', inline: <<-SHELL
    set -e

    # Override sources.list for faster downloads (for people outside of the US).
    echo deb http://httpredir.debian.org/debian/ stretch main > /etc/apt/sources.list
    echo deb-src http://httpredir.debian.org/debian/ stretch main >> /etc/apt/sources.list
    echo deb http://security.debian.org/debian-security stretch/updates main >> /etc/apt/sources.list
    echo deb-src http://security.debian.org/debian-security stretch/updates main >> /etc/apt/sources.list

    sudo apt-get update
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
      astyle \
      cmake \
      dosfstools \
      gcc-multilib \
      gdb \
      genisoimage \
      git \
      grub-legacy \
      mtools \
      nasm \
      qemu

    echo 'drive u: file="/vagrant/floppy.img"' > /etc/mtools.conf

    echo 'cd /vagrant' >> /home/vagrant/.bashrc
    echo '-U' > /home/vagrant/.astylerc
    echo '-H' >> /home/vagrant/.astylerc
    echo '-S' >> /home/vagrant/.astylerc

    echo target remote localhost:1234 > /home/vagrant/.gdbinit
  SHELL
end
