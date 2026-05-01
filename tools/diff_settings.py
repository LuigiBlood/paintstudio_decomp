#!/usr/bin/env python3

def apply(config, args):
    config['baseimg'] = 'dmpj.d64'
    config['myimg'] = 'build/dmpj.d64'
    config['mapfile'] = 'build/dmpj.map'
    config['source_directories'] = ['.']
    config['makeflags'] = ['COMPARE=0']