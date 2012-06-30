#!/bin/bash

kvm -serial mon:stdio -cdrom kernel.iso -hda test.img
