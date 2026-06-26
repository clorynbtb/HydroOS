import React, { useState, useEffect } from 'react';

interface WidgetPanelProps {
  cpuUsage: number;
  ramUsage: string;
}

/* Mac Classic retro card — raised bevel panel, no rounded corners */
const macCard: React.CSSProperties = {
  background: '#EFEBE9',
  border: '1px solid #5D4037',
  boxShadow: `
    inset 1px 1px 0 #FFFFFF,
    inset -1px -1px 0 #8D6E63,
    1px 1px 0 #BCAAA4
  `,
  padding: '8px 10px',
};

export default function WidgetPanel({ cpuUsage, ramUsage }: WidgetPanelProps) {
  const [sparkHistory, setSparkHistory] = useState<number[]>(Array(12).fill(15));

  useEffect(() => {
    const interval = setInterval(() => {
      setSparkHistory(prev => [...prev.slice(1), cpuUsage]);
    }, 1200);
    return () => clearInterval(interval);
  }, [cpuUsage]);

  return (
    <div className="absolute top-10 right-4 w-44 space-y-2 select-none font-sans z-10 pointer-events-auto">

      {/* Kernel Architecture */}
      <div style={macCard}>
        <p className="text-[8px] font-bold uppercase tracking-wider mb-0.5" style={{ color: '#8D6E63' }}>Kernel Arch</p>
        <p className="text-[10px] font-bold font-mono" style={{ color: '#3E2723' }}>x86_64-none-elf</p>
      </div>

      {/* Bootloader */}
      <div style={macCard}>
        <p className="text-[8px] font-bold uppercase tracking-wider mb-0.5" style={{ color: '#8D6E63' }}>Bootloader</p>
        <p className="text-[10px] font-bold font-mono flex items-center gap-1.5" style={{ color: '#3E2723' }}>
          {/* Mac Classic active dot — square, not circle */}
          <span
            className="animate-pulse"
            style={{ width: 5, height: 5, background: '#3E2723', display: 'inline-block', flexShrink: 0 }}
          />
          Limine v5.18.2
        </p>
      </div>

      {/* Toolchain */}
      <div style={macCard}>
        <p className="text-[8px] font-bold uppercase tracking-wider mb-0.5" style={{ color: '#8D6E63' }}>Build Toolchain</p>
        <p className="text-[10px] font-bold font-mono" style={{ color: '#3E2723' }}>LLVM CLANG + lld</p>
      </div>

      {/* CPU Spark Monitor — sunken panel (pressed style) */}
      <div
        style={{
          background: '#E0D4C3',
          border: '1px solid #5D4037',
          boxShadow: `inset 1px 1px 0 #8D6E63, inset -1px -1px 0 #FFFFFF`,
          padding: '8px 10px',
        }}
      >
        <div className="flex justify-between items-center mb-1.5">
          <p className="text-[8px] font-bold uppercase tracking-wider" style={{ color: '#8D6E63' }}>Virtual Load</p>
          <p className="text-[9px] font-black font-mono" style={{ color: '#3E2723' }}>{cpuUsage}%</p>
        </div>

        {/* Spark bar chart — Mac Classic rectangular bars */}
        <div className="flex items-end gap-[2px]" style={{ height: 24 }}>
          {sparkHistory.map((val, idx) => {
            const h = Math.max(10, Math.min(100, val));
            const barColor = val > 70 ? '#BF360C' : val > 40 ? '#8D6E63' : '#5D4037';
            return (
              <div
                key={idx}
                style={{
                  height: `${h}%`,
                  background: barColor,
                  flex: 1,
                  transition: 'height 0.3s ease',
                }}
              />
            );
          })}
        </div>
      </div>
    </div>
  );
}
