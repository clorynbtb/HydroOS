import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'motion/react';

interface BootScreenProps {
  onBootComplete: () => void;
  rebootTrigger: number;
}

export default function BootScreen({ onBootComplete, rebootTrigger }: BootScreenProps) {
  const [stage, setStage] = useState<'limine' | 'hydrogen' | 'loading' | 'success'>('limine');
  const [limineLogs, setLimineLogs] = useState<string[]>([]);
  const [progress, setProgress] = useState(0);
  const [currentKernelAction, setCurrentKernelAction] = useState('Booting HydroOS kernel...');

  // Sound chime effect
  const playStartupChime = () => {
    try {
      const AudioCtx = window.AudioContext || (window as any).webkitAudioContext;
      if (!AudioCtx) return;
      const ctx = new AudioCtx();
      
      // Notion Chime: simple neat double-pulse sine wave (F4, A4, C5)
      const notes = [349.23, 440.00, 523.25];
      const durations = [0.12, 0.12, 0.5];
      let time = ctx.currentTime;
      
      notes.forEach((freq, idx) => {
        const osc = ctx.createOscillator();
        const gain = ctx.createGain();
        
        osc.type = 'sine';
        osc.frequency.value = freq;
        
        gain.gain.setValueAtTime(0, time);
        gain.gain.linearRampToValueAtTime(0.12, time + 0.02);
        gain.gain.exponentialRampToValueAtTime(0.001, time + durations[idx]);
        
        osc.connect(gain);
        gain.connect(ctx.destination);
        
        osc.start(time);
        osc.stop(time + durations[idx]);
        
        time += 0.08;
      });
    } catch (e) {
      console.log('Audio contextual startup sound blocked or unsupported', e);
    }
  };

  const limineBootMessages = [
    "Limine Bootloader v5.18.2-x86_64",
    "Copyright (C) 2022-2026 Limine Contributors",
    "Detecting memory map...",
    "Allocating page tables for x86_64 long mode...",
    "Resolution queried: 1024x768 (32-bit Framebuffer)",
    "Handoff parameters compiled via LLD linker...",
    "Loading ELF64 kernel.elf at physical address 0xffffffff80100000...",
    "Verifying kernel checksum... [OK]",
    "Executing kernel entry point _start()..."
  ];

  const kernelActions = [
    { threshold: 0, text: "[kernel.c] Booting Hydrogen Core..." },
    { threshold: 15, text: "[kernel.c] Initializing Oxygen UI (oxygen.c)..." },
    { threshold: 30, text: "[oxygen.c] Mapping 1024x768 framebuffer from Limine..." },
    { threshold: 45, text: "[oxygen.c] Loaded 8x8 bitmap font glyphs (ASCII 0-255)" },
    { threshold: 60, text: "[db.c] Initializing Dubnium Docker Engine..." },
    { threshold: 75, text: "[db.c] Mounting virtual Docker registry..." },
    { threshold: 90, text: "[db.c] Creating system containers: nginx-stable, postgres-db..." },
    { threshold: 100, text: "[kernel.c] Handing off controller to Oxygen Desktop..." }
  ];

  useEffect(() => {
    // Reset state for reboot
    setStage('limine');
    setLimineLogs([]);
    setProgress(0);
    setCurrentKernelAction('Booting HydroOS kernel...');

    let isMounted = true;
    
    // Simulate Limine logs printing one by one
    let logIndex = 0;
    const printLimineLog = () => {
      if (!isMounted) return;
      if (logIndex < limineBootMessages.length) {
        setLimineLogs(prev => [...prev, limineBootMessages[logIndex]]);
        logIndex++;
        setTimeout(printLimineLog, 120);
      } else {
        // Transition to Hydrogen splash
        setTimeout(() => {
          if (isMounted) setStage('hydrogen');
        }, 200);
      }
    };

    setTimeout(printLimineLog, 100);

    return () => {
      isMounted = false;
    };
  }, [rebootTrigger]);

  // Hydrogen Loading progress simulation
  useEffect(() => {
    if (stage !== 'hydrogen') return;
    
    let isMounted = true;
    let currentProgress = 0;
    
    const interval = setInterval(() => {
      if (!isMounted) return;
      currentProgress += Math.floor(Math.random() * 6) + 2;
      
      if (currentProgress >= 100) {
        currentProgress = 100;
        setProgress(100);
        clearInterval(interval);
        
        // Play startup sound and trigger completion
        playStartupChime();
        setTimeout(() => {
          if (isMounted) {
            setStage('success');
            setTimeout(onBootComplete, 300);
          }
        }, 500);
      } else {
        setProgress(currentProgress);
        // Find corresponding action label
        const currentAction = kernelActions.reduce((acc, act) => {
          if (currentProgress >= act.threshold) return act.text;
          return acc;
        }, kernelActions[0].text);
        setCurrentKernelAction(currentAction);
      }
    }, 45);

    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, [stage]);

  return (
    <div id="boot-screen" className="absolute inset-0 bg-[#FFFFFF] text-[#37352F] font-mono flex flex-col justify-between p-12 select-none overflow-hidden h-[768px] w-[1024px] border border-[#E9E9E6]">
      <AnimatePresence mode="wait">
        {stage === 'limine' && (
          <motion.div
            key="limine-stage"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="flex-1 flex flex-col justify-start text-[11px] leading-relaxed space-y-1.5 max-w-2xl font-mono text-[#37352F]"
          >
            {limineLogs.map((log, idx) => (
              <div key={idx} className="flex gap-3">
                <span className="text-[#9B9A97]">[{new Date().toISOString().split('T')[1].slice(0, 12)}]</span>
                <span className="font-medium">{log}</span>
              </div>
            ))}
            <motion.span
              animate={{ opacity: [1, 0, 1] }}
              transition={{ repeat: Infinity, duration: 0.8 }}
              className="text-[#37352F] font-bold ml-16"
            >
              _
            </motion.span>
          </motion.div>
        )}

        {(stage === 'hydrogen' || stage === 'success') && (
          <motion.div
            key="hydrogen-stage"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="flex-1 flex flex-col items-center justify-center space-y-12"
          >
            {/* Hydrogen Elegant Thin Line Logo - Notion Minimalist */}
            <motion.div
              initial={{ scale: 0.9, opacity: 0 }}
              animate={{ scale: 1, opacity: 1 }}
              className="w-24 h-24 border border-[#E9E9E6] bg-[#F7F7F5] flex items-center justify-center select-none rounded-lg"
            >
              <div className="relative w-14 h-14 flex items-center justify-center">
                {/* Left vertical bar */}
                <div className="absolute left-0 top-0 bottom-0 w-2 bg-[#37352F]" />
                {/* Right vertical bar */}
                <div className="absolute right-0 top-0 bottom-0 w-2 bg-[#37352F]" />
                {/* Center bridge */}
                <div className="absolute left-2 right-2 h-2 top-[26px] bg-[#37352F]" />
              </div>
            </motion.div>

            {/* Custom Notion Loading bar */}
            <div className="w-[320px] flex flex-col items-center gap-4">
              <div className="w-full h-[2px] bg-[#E9E9E6] overflow-hidden relative">
                <motion.div
                  className="absolute left-0 top-0 bottom-0 bg-[#37352F]"
                  style={{ width: `${progress}%` }}
                  transition={{ ease: 'easeOut' }}
                />
              </div>

              {/* Loader labels */}
              <div className="flex flex-col items-center gap-1 h-12 justify-center">
                <span className="text-[10px] tracking-[0.15em] text-[#9B9A97] font-bold">
                  HYDROGEN CORE
                </span>
                <span className="text-[11px] text-[#37352F] font-medium font-mono">
                  {currentKernelAction} ({progress}%)
                </span>
              </div>
            </div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Boot Footer Info */}
      <div className="flex justify-between items-center text-[10px] text-[#9B9A97] border-t border-[#E9E9E6] pt-4">
        <span>ARCH: x86_64-none-elf</span>
        <span className="font-semibold">LLD / Clang Toolchain v18.1.0</span>
        <span>Limine 5.x OS Loader</span>
      </div>
    </div>
  );
}
