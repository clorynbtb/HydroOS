import React from 'react';
import { motion } from 'motion/react';
import { Terminal, Code, Trash2 } from 'lucide-react';

interface DockProps {
  onOpenWindow: (windowId: string) => void;
  activeWindows: { [key: string]: boolean };
  onClearContainers: () => void;
}

/* Mac Classic retro bevel button style — raised 3D mechanical key */
const retroButtonStyle = (bg = '#EFEBE9'): React.CSSProperties => ({
  background: bg,
  border: '1px solid #5D4037',
  boxShadow: `
    inset 1px 1px 0 #FFFFFF,
    inset -1px -1px 0 #5D4037,
    1px 1px 0 #BCAAA4
  `,
});

const retroButtonPressedStyle = (bg = '#D7CCC8'): React.CSSProperties => ({
  background: bg,
  border: '1px solid #5D4037',
  boxShadow: `
    inset 1px 1px 0 #5D4037,
    inset -1px -1px 0 #FFFFFF
  `,
});

export default function Dock({ onOpenWindow, activeWindows, onClearContainers }: DockProps) {
  const dockItems = [
    {
      id: 'docker-manager',
      label: 'Dubnium [Db]',
      symbol: 'Db',
      isCustom: true,
      bg: '#E0D4C3',
      activeColor: '#3E2723',
    },
    {
      id: 'compiler',
      label: 'Clang Compiler',
      symbol: <Code className="w-4 h-4" style={{ color: '#5D4037' }} />,
      isCustom: false,
      bg: '#EFEBE9',
      activeColor: '#3E2723',
    },
    {
      id: 'terminal',
      label: 'TTY Terminal',
      symbol: <Terminal className="w-4 h-4" style={{ color: '#5D4037' }} />,
      isCustom: false,
      bg: '#EFEBE9',
      activeColor: '#3E2723',
    },
  ];

  return (
    <footer className="h-20 flex items-center justify-center mb-3 select-none font-sans relative z-40">
      {/* Mac Classic Control Tray — raised 3D panel */}
      <div
        className="flex items-center gap-2 px-3 py-2"
        style={{
          background: '#EFEBE9',
          border: '1px solid #5D4037',
          /* Mac Classic raised tray bevel */
          boxShadow: `
            inset 1px 1px 0 #FFFFFF,
            inset -1px -1px 0 #5D4037,
            2px 2px 0 #D7CCC8,
            3px 3px 0 #BCAAA4
          `,
        }}
      >
        {dockItems.map((item) => {
          const isOpen = activeWindows[item.id];
          return (
            <div key={item.id} className="relative group flex flex-col items-center gap-1">
              {/* Mac Classic Tooltip */}
              <div
                className="absolute -top-9 left-1/2 -translate-x-1/2 text-[9px] font-bold px-2 py-0.5 whitespace-nowrap pointer-events-none opacity-0 group-hover:opacity-100 transition-opacity font-sans z-50"
                style={{
                  ...retroButtonStyle('#FFFFFF'),
                  color: '#3E2723',
                }}
              >
                {item.label}
              </div>

              {/* Mac Classic 3D App Button — mechanical key press */}
              <motion.button
                onClick={() => onOpenWindow(item.id)}
                whileTap={{ scale: 0.96 }}
                className="w-11 h-11 flex items-center justify-center cursor-pointer transition-colors"
                style={isOpen ? retroButtonPressedStyle(item.bg) : retroButtonStyle(item.bg)}
              >
                {item.isCustom ? (
                  <span className="font-mono font-black text-sm" style={{ color: '#3E2723' }}>
                    {item.symbol as string}
                  </span>
                ) : (
                  item.symbol
                )}
              </motion.button>

              {/* Active dot — small filled square (Mac Classic style) */}
              {isOpen && (
                <div
                  className="w-1.5 h-1.5"
                  style={{ background: '#3E2723' }}
                />
              )}
              {!isOpen && <div className="w-1.5 h-1.5" />}
            </div>
          );
        })}

        {/* Mac Classic Separator — engraved line */}
        <div className="mx-1 self-stretch flex flex-col justify-center">
          <div style={{ width: 1, height: 28, background: '#8D6E63', boxShadow: '1px 0 0 #FFFFFF' }} />
        </div>

        {/* Trash / Prune Button */}
        <div className="relative group flex flex-col items-center gap-1">
          <div
            className="absolute -top-9 left-1/2 -translate-x-1/2 text-[9px] font-bold px-2 py-0.5 whitespace-nowrap pointer-events-none opacity-0 group-hover:opacity-100 transition-opacity font-sans z-50"
            style={{ ...retroButtonStyle('#FFFFFF'), color: '#3E2723' }}
          >
            Prune Containers
          </div>
          <motion.button
            onClick={() => onClearContainers()}
            whileTap={{ scale: 0.96 }}
            className="w-11 h-11 flex items-center justify-center cursor-pointer transition-colors group/trash"
            style={retroButtonStyle('#EFEBE9')}
          >
            <Trash2 className="w-4 h-4 group-hover/trash:text-[#BF360C] transition-colors" style={{ color: '#8D6E63' }} />
          </motion.button>
          <div className="w-1.5 h-1.5" />
        </div>
      </div>
    </footer>
  );
}
