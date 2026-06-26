import React, { useState, useEffect } from 'react';
import { Cpu, HelpCircle, ShieldAlert } from 'lucide-react';

interface WidgetPanelProps {
  cpuUsage: number;
  ramUsage: string;
}

export default function WidgetPanel({ cpuUsage, ramUsage }: WidgetPanelProps) {
  const [sparkHistory, setSparkHistory] = useState<number[]>(Array(12).fill(15));

  // Periodically log cpu usage into the spark chart history
  useEffect(() => {
    const interval = setInterval(() => {
      setSparkHistory(prev => {
        const next = [...prev.slice(1), cpuUsage];
        return next;
      });
    }, 1200);
    return () => clearInterval(interval);
  }, [cpuUsage]);

  return (
    <div className="absolute top-12 right-6 w-48 space-y-2.5 select-none font-sans relative z-10 pointer-events-auto text-[#37352F]">
      {/* Target Spec */}
      <div className="bg-[#FFFFFF] p-3 border border-[#E9E9E6] rounded-lg shadow-sm transition-all hover:shadow-md">
        <p className="text-[8px] text-[#9B9A97] mb-0.5 uppercase tracking-wider font-bold">Kernel Architecture</p>
        <p className="text-[11px] text-[#37352F] font-bold font-mono">x86_64-none-elf</p>
      </div>

      {/* Bootloader info */}
      <div className="bg-[#FFFFFF] p-3 border border-[#E9E9E6] rounded-lg shadow-sm transition-all hover:shadow-md">
        <p className="text-[8px] text-[#9B9A97] mb-0.5 uppercase tracking-wider font-bold">Bootloader Mode</p>
        <p className="text-[11px] text-[#37352F] font-bold flex items-center gap-1.5 font-mono">
          <span className="w-1.5 h-1.5 bg-[#0F7B5C] rounded-full animate-pulse" />
          Limine v5.18.2
        </p>
      </div>

      {/* Compiler Specs */}
      <div className="bg-[#FFFFFF] p-3 border border-[#E9E9E6] rounded-lg shadow-sm transition-all hover:shadow-md">
        <p className="text-[8px] text-[#9B9A97] mb-0.5 uppercase tracking-wider font-bold">Build Toolchain</p>
        <p className="text-[11px] text-[#37352F] font-bold font-mono">LLVM CLANG - lld</p>
      </div>

      {/* Dynamic Load Sparks */}
      <div className="bg-[#FFFFFF] p-3 border border-[#E9E9E6] rounded-lg shadow-sm transition-all hover:shadow-md space-y-1.5">
        <p className="text-[8px] text-[#9B9A97] uppercase tracking-wider font-bold flex justify-between">
          <span>Virtual Load</span>
          <span className="text-[#37352F] font-bold font-mono">{cpuUsage}%</span>
        </p>

        {/* CSS Spark Mini Bar Chart */}
        <div className="h-6 flex items-end gap-[2px] pt-1">
          {sparkHistory.map((val, idx) => {
            // Map CPU percentage to height (max 24px)
            const heightPercent = Math.max(10, Math.min(100, val));
            return (
              <div
                key={idx}
                style={{ height: `${heightPercent}%` }}
                className={`w-full rounded-t-[1px] transition-all duration-300 ${
                  val > 70 
                    ? 'bg-[#EB5757]/80' 
                    : val > 40 
                      ? 'bg-[#D9730D]/80' 
                      : 'bg-[#0F7B5C]/70'
                }`}
              />
            );
          })}
        </div>
      </div>
    </div>
  );
}
