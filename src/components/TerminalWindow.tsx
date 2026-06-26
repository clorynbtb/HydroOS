import React, { useState, useRef, useEffect } from 'react';
import { Container, LogEntry } from '../types';

interface TerminalWindowProps {
  containers: Container[];
  logs: LogEntry[];
  onAddLog: (text: string, type: LogEntry['type']) => void;
  onClearLogs: () => void;
  onRunContainer: (image: string) => void;
  onPauseContainer: (id: string) => void;
  onStopContainer: (id: string) => void;
  onTriggerReboot: () => void;
  onCompileCode: (fileName: string) => void;
}

export default function TerminalWindow({
  containers,
  logs,
  onAddLog,
  onClearLogs,
  onRunContainer,
  onPauseContainer,
  onStopContainer,
  onTriggerReboot,
  onCompileCode
}: TerminalWindowProps) {
  const [inputValue, setInputValue] = useState('');
  const [commandHistory, setCommandHistory] = useState<string[]>([]);
  const [historyIndex, setHistoryIndex] = useState(-1);
  const terminalBottomRef = useRef<HTMLDivElement | null>(null);

  useEffect(() => {
    // Auto-scroll to bottom of logs
    terminalBottomRef.current?.scrollIntoView({ behavior: 'smooth' });
  }, [logs]);

  const handleCommandSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    const command = inputValue.trim();
    if (!command) return;

    // Save in command history
    const newHistory = [...commandHistory, command];
    setCommandHistory(newHistory);
    setHistoryIndex(newHistory.length);

    // Add command to log
    onAddLog(`hydro@notion-os:~$ ${command}`, 'input');
    setInputValue('');

    // Execute parser
    const args = command.split(' ');
    const baseCmd = args[0].toLowerCase();

    switch (baseCmd) {
      case 'help':
        onAddLog('Available commands:', 'info');
        onAddLog('  help                      Show this help listing', 'info');
        onAddLog('  clear                     Clear the screen buffer', 'info');
        onAddLog('  neofetch                  Display hardware and system diagnostics', 'info');
        onAddLog('  clang <target> <file>     Compile a source file (e.g., "clang -target x86_64-none-elf kernel.c")', 'info');
        onAddLog('  docker ps                 List active, paused or stopped containers', 'info');
        onAddLog('  docker images             List cached docker layers in Limine cache', 'info');
        onAddLog('  docker run <image>        Spin up a container (nginx-stable, postgres-db, alpine-linux, redis-kv)', 'info');
        onAddLog('  docker pause <id>         Pause a container process', 'info');
        onAddLog('  docker stop <id>          Stop/Terminate a container process', 'info');
        onAddLog('  docker rm <id>            Prune/Delete a container process', 'info');
        onAddLog('  reboot                    Trigger flash reboot of HydroOS', 'info');
        break;

      case 'clear':
        onClearLogs();
        break;

      case 'neofetch':
        onAddLog('     _    _           _       _ ', 'success');
        onAddLog('    | |  | |         | |     | |', 'success');
        onAddLog('    | |__| |_   _  __| |_ __ ___| |__ ', 'success');
        onAddLog('    |  __  | | | |/ _` | \'__/ _ \\  _ \\', 'success');
        onAddLog('    | |  | | |_| | (_| | | | (_) | |_) |', 'success');
        onAddLog('    |_|  |_|\\__, |\\__,_|_|  \\___/|_.__/', 'success');
        onAddLog('             __/ |                      ', 'success');
        onAddLog('            |___/                       ', 'success');
        onAddLog('----------------------------------------', 'info');
        onAddLog('OS: HydroOS v1.0.4 - 64-Bit x86_64 ELF', 'info');
        onAddLog('Kernel: Hydrogen Core 1.0.0-none-elf', 'info');
        onAddLog('UI Render Engine: Oxygen v1.0 (8x8 custom font)', 'info');
        onAddLog('Bootloader: Limine Bootloader v5.x', 'info');
        onAddLog('Toolchain: CLANG 18.1.0 (Target: x86_64-none-elf)', 'info');
        onAddLog('Linker: LLD Linker (m elf_x86_64)', 'info');
        onAddLog(`Containers: ${containers.filter(c => c.status === 'running').length} running / ${containers.length} total`, 'info');
        onAddLog('RAM Allocation: 1.2GB/4.0GB virtualised', 'info');
        break;

      case 'reboot':
        onAddLog('[system] Terminating container schedulers...', 'warning');
        onAddLog('[system] Unmapping Limine framebuffer...', 'warning');
        onAddLog('[system] Rebooting via system controller...', 'success');
        setTimeout(() => {
          onTriggerReboot();
        }, 1000);
        break;

      case 'clang':
        // Handle clang compile
        if (args.includes('kernel.c') || args.includes('oxygen.c') || args.includes('db.c')) {
          const fileToCompile = args.includes('kernel.c') ? 'kernel.c' : args.includes('oxygen.c') ? 'oxygen.c' : 'db.c';
          onAddLog(`[clang] Invoking LLVM target compiler...`, 'info');
          onAddLog(`[clang] clang -target x86_64-none-elf -ffreestanding -O2 -c ${fileToCompile} -o ${fileToCompile.replace('.c', '.o')}`, 'info');
          
          setTimeout(() => {
            onCompileCode(fileToCompile);
          }, 800);
        } else {
          onAddLog('Error: Usage: clang -target x86_64-none-elf <kernel.c|oxygen.c|db.c>', 'error');
        }
        break;

      case 'docker':
        const subAction = args[1] ? args[1].toLowerCase() : '';
        if (subAction === 'ps') {
          onAddLog('CONTAINER ID   IMAGE           STATUS         PORT       MEMORY', 'info');
          if (containers.length === 0) {
            onAddLog('(No active containers registered)', 'warning');
          } else {
            containers.forEach(c => {
              const statusStr = c.status === 'running' ? '● Running' : c.status === 'paused' ? '◒ Paused' : '○ Stopped';
              onAddLog(`${c.id.padEnd(14)}${c.image.padEnd(16)}${statusStr.padEnd(15)}${c.port.padEnd(11)}${c.memory}`, 'info');
            });
          }
        } else if (subAction === 'images') {
          onAddLog('REPOSITORY          TAG       IMAGE ID       SIZE', 'info');
          onAddLog('nginx-stable        latest    a82fb1012ca4   142MB', 'info');
          onAddLog('postgres-db         15.3      db71a8bc5190   379MB', 'info');
          onAddLog('alpine-linux        3.18      f73c68b7c42b   7.05MB', 'info');
          onAddLog('redis-kv            7.0       4ef2ca1bc01d   113MB', 'info');
        } else if (subAction === 'run') {
          const imgName = args[2];
          if (!imgName) {
            onAddLog('Error: Specify image name. E.g., "docker run nginx-stable"', 'error');
          } else {
            onAddLog(`[db.c] Pulling docker image layer for: ${imgName}...`, 'info');
            setTimeout(() => {
              onRunContainer(imgName);
            }, 500);
          }
        } else if (subAction === 'pause') {
          const containerId = args[2];
          if (!containerId) {
            onAddLog('Error: Specify container ID. E.g., "docker pause db-7f31a"', 'error');
          } else {
            onPauseContainer(containerId);
          }
        } else if (subAction === 'stop') {
          const containerId = args[2];
          if (!containerId) {
            onAddLog('Error: Specify container ID. E.g., "docker stop db-7f31a"', 'error');
          } else {
            onStopContainer(containerId);
          }
        } else if (subAction === 'rm') {
          const containerId = args[2];
          if (!containerId) {
            onAddLog('Error: Specify container ID to delete.', 'error');
          } else {
            onStopContainer(containerId); // Stops first then triggers log
            onAddLog(`[db.c] Purged container registry reference: ${containerId}`, 'success');
          }
        } else {
          onAddLog('Error: Unknown docker command. Options: ps, images, run, pause, stop, rm', 'error');
        }
        break;

      default:
        onAddLog(`Command not found: "${baseCmd}". Type "help" for instructions.`, 'error');
        break;
    }
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === 'ArrowUp') {
      e.preventDefault();
      if (historyIndex > 0) {
        const nextIndex = historyIndex - 1;
        setHistoryIndex(nextIndex);
        setInputValue(commandHistory[nextIndex]);
      }
    } else if (e.key === 'ArrowDown') {
      e.preventDefault();
      if (historyIndex < commandHistory.length - 1) {
        const nextIndex = historyIndex + 1;
        setHistoryIndex(nextIndex);
        setInputValue(commandHistory[nextIndex]);
      } else {
        setHistoryIndex(commandHistory.length);
        setInputValue('');
      }
    }
  };

  return (
    <div className="p-4 bg-[#FFFFFF] text-[#37352F] font-mono text-[12px] h-[340px] flex flex-col justify-between select-text selection:bg-[#EDF6F3] selection:text-[#0F7B5C]">
      {/* Logs Scroll Container */}
      <div className="flex-1 overflow-y-auto space-y-1.5 scrollbar-thin">
        <div className="text-[#9B9A97] mb-2.5 border-b border-[#E9E9E6] pb-1.5 flex justify-between items-center text-[10px] font-bold">
          <span>HYDROGEN CORE VIRTUAL SHELL v1.0.4 (tty1)</span>
          <span className="text-[#0F7B5C]">LLD LINKER ACTIVE</span>
        </div>

        {logs.map((log, index) => {
          let textClass = 'text-[#37352F]';
          if (log.type === 'system') textClass = 'text-[#37352F] font-bold';
          if (log.type === 'info') textClass = 'text-[#9B9A97]';
          if (log.type === 'success') textClass = 'text-[#0F7B5C] font-semibold';
          if (log.type === 'warning') textClass = 'text-[#D9730D] font-semibold';
          if (log.type === 'error') textClass = 'text-[#EB5757] font-bold';
          if (log.type === 'input') textClass = 'text-[#37352F] font-bold';

          return (
            <div key={index} className="leading-relaxed whitespace-pre-wrap">
              <span className={textClass}>{log.text}</span>
            </div>
          );
        })}
        <div ref={terminalBottomRef} />
      </div>

      {/* Input Prompt Form */}
      <form onSubmit={handleCommandSubmit} className="flex gap-2 border-t border-[#E9E9E6] pt-2.5 mt-2 bg-[#FFFFFF]">
        <span className="text-[#0F7B5C] font-extrabold shrink-0">hydro@notion-os:~$</span>
        <input
          type="text"
          value={inputValue}
          onChange={(e) => setInputValue(e.target.value)}
          onKeyDown={handleKeyDown}
          autoComplete="off"
          autoFocus
          className="flex-grow bg-transparent text-[#37352F] outline-none font-mono caret-[#37352F] border-none p-0 text-[12px]"
          placeholder="Type 'help' to begin..."
        />
      </form>
    </div>
  );
}
