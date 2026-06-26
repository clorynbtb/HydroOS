import React, { useState, useEffect, useRef } from 'react';
import { Container, LogEntry, CodeFile, OSState, WindowState } from './types';
import { INITIAL_CODE_FILES } from './constants/initialCode';
import BootScreen from './components/BootScreen';
import TopMenuBar from './components/TopMenuBar';
import Dock from './components/Dock';
import WindowWrapper from './components/WindowWrapper';
import TerminalWindow from './components/TerminalWindow';
import DockerManagerWindow from './components/DockerManagerWindow';
import CodeEditorWindow from './components/CodeEditorWindow';
import WidgetPanel from './components/WidgetPanel';

export default function App() {
  // OS Boot sequence state
  const [osState, setOsState] = useState<OSState>('booting');
  const [rebootTrigger, setRebootTrigger] = useState(0);

  // Virtual source files state
  const [codeFiles, setCodeFiles] = useState<CodeFile[]>(INITIAL_CODE_FILES);

  // Active virtual containers inside Dubnium engine memory registry
  const [containers, setContainers] = useState<Container[]>([
    {
      id: 'db-7f31a',
      image: 'nginx-stable',
      status: 'running',
      memory: '45MB',
      cpu: 12,
      port: '80',
      created: '10 mins ago',
      appName: 'Helium Web Server',
      category: 'web'
    },
    {
      id: 'li-00x9z',
      image: 'postgres-db',
      status: 'paused',
      memory: '128MB',
      cpu: 0,
      port: '5432',
      created: '35 mins ago',
      appName: 'Lithium database instance',
      category: 'database'
    },
  ]);

  // Console output history logs
  const [logs, setLogs] = useState<LogEntry[]>([
    { text: '[kernel.c] Initializing Oxygen UI (oxygen.c)...', type: 'system', timestamp: '00:07:42' },
    { text: '[oxygen.c] Mapping 1024x768 framebuffer from Limine...', type: 'info', timestamp: '00:07:42' },
    { text: '[oxygen.c] Loaded 8x8 bitmap font glyphs (ASCII 0-255)', type: 'info', timestamp: '00:07:43' },
    { text: '[db.c] Dubnium Engine linked with LLD. Ready.', type: 'success', timestamp: '00:07:43' },
  ]);

  // Window list states
  const [openWindows, setOpenWindows] = useState<{ [key: string]: boolean }>({
    'docker-manager': true,
    'compiler': false,
    'terminal': false,
  });

  const [minimizedWindows, setMinimizedWindows] = useState<{ [key: string]: boolean }>({});

  // Focus z-indices tracker
  const [windowZIndex, setWindowZIndex] = useState<{ [key: string]: number }>({
    'docker-manager': 10,
    'compiler': 5,
    'terminal': 5,
  });

  const [topZ, setTopZ] = useState(10);

  // CPU / RAM Live Diagnostics Metrics
  const [cpuUsage, setCpuUsage] = useState(15);
  const [ramUsage, setRamUsage] = useState('1.2GB');

  const desktopRef = useRef<HTMLDivElement | null>(null);

  // Fluctuate CPU metrics in background naturally
  useEffect(() => {
    if (osState !== 'desktop') return;

    const interval = setInterval(() => {
      // Calculate CPU base on active containers
      const runningCount = containers.filter(c => c.status === 'running').length;
      const baseLoad = 10 + (runningCount * 14);
      // Small randomized wobble
      const wobble = Math.floor(Math.random() * 8) - 4;
      setCpuUsage(Math.max(5, Math.min(99, baseLoad + wobble)));

      // Dynamic RAM summary based on allocated containers
      let baseRamMb = 256; // core kernel overhead
      containers.forEach(c => {
        if (c.status !== 'stopped') {
          const mbVal = parseInt(c.memory.replace('MB', '')) || 0;
          baseRamMb += mbVal;
        }
      });
      const gbVal = (baseRamMb / 1024).toFixed(2);
      setRamUsage(`${gbVal}GB`);
    }, 1500);

    return () => clearInterval(interval);
  }, [containers, osState]);

  // Handle log appenders
  const handleAddLog = (text: string, type: LogEntry['type']) => {
    const time = new Date().toTimeString().split(' ')[0];
    setLogs(prev => [...prev, { text, type, timestamp: time }]);
  };

  const handleClearLogs = () => {
    setLogs([]);
  };

  // Trigger hard reboot cycle of HydroOS
  const handleReboot = () => {
    setOsState('rebooting');
    handleAddLog('[kernel] Reboot command received.', 'system');
    setTimeout(() => {
      setRebootTrigger(prev => prev + 1);
      setOsState('booting');
    }, 600);
  };

  // Window Focus trigger
  const handleFocusWindow = (id: string) => {
    const nextZ = topZ + 1;
    setTopZ(nextZ);
    setWindowZIndex(prev => ({ ...prev, [id]: nextZ }));
    setMinimizedWindows(prev => ({ ...prev, [id]: false }));
  };

  const handleOpenWindow = (id: string) => {
    setOpenWindows(prev => ({ ...prev, [id]: true }));
    handleFocusWindow(id);
  };

  const handleCloseWindow = (id: string) => {
    setOpenWindows(prev => ({ ...prev, [id]: false }));
  };

  const handleMinimizeWindow = (id: string) => {
    setMinimizedWindows(prev => ({ ...prev, [id]: true }));
  };

  // Docker command simulations
  const handleRunContainer = (image: string, customPort = '8080', customRam = '64MB') => {
    const exists = containers.find(c => c.image === image);
    let finalId = '';
    if (exists && exists.status === 'stopped') {
      // Resume existing
      setContainers(prev => prev.map(c => c.image === image ? { ...c, status: 'running' } : c));
      finalId = exists.id;
    } else {
      // Generate new
      const randId = 'db-' + Math.floor(Math.random() * 16777215).toString(16).slice(0, 5);
      finalId = randId;
      const isWeb = image.includes('nginx');
      const isDb = image.includes('postgres');
      const isRedis = image.includes('redis');
      
      const newContainer: Container = {
        id: randId,
        image,
        status: 'running',
        memory: customRam,
        cpu: Math.floor(Math.random() * 12) + 6,
        port: customPort,
        created: 'Just now',
        appName: isWeb ? 'Helium Web Server' : isDb ? 'Lithium database instance' : isRedis ? 'Redis KV memory db' : 'Alpine shell daemon',
        category: isWeb ? 'web' : isDb ? 'database' : isRedis ? 'utility' : 'system'
      };
      setContainers(prev => [...prev, newContainer]);
    }
    handleAddLog(`[db.c] Spun up container ${finalId} with image: ${image}`, 'success');
  };

  const handlePauseContainer = (id: string) => {
    setContainers(prev => prev.map(c => c.id === id ? { ...c, status: 'paused', cpu: 0 } : c));
    handleAddLog(`[db.c] Paused execution process on container: ${id}`, 'warning');
  };

  const handleResumeContainer = (id: string) => {
    setContainers(prev => prev.map(c => c.id === id ? { ...c, status: 'running', cpu: Math.floor(Math.random() * 10) + 4 } : c));
    handleAddLog(`[db.c] Resumed CPU scheduler for container: ${id}`, 'success');
  };

  const handleStopContainer = (id: string) => {
    setContainers(prev => prev.map(c => c.id === id ? { ...c, status: 'stopped', cpu: 0 } : c));
    handleAddLog(`[db.c] Stopped/Halted process memory loop: ${id}`, 'warning');
  };

  const handleRemoveContainer = (id: string) => {
    setContainers(prev => prev.filter(c => c.id !== id));
    handleAddLog(`[db.c] Evicted container context registry: ${id}`, 'success');
  };

  const handleClearContainers = () => {
    setContainers([]);
    handleAddLog(`[db.c] Pruned all stopped and dormant Docker layers from core memory`, 'success');
  };

  // Save modified source files
  const handleSaveFile = (fileName: string, content: string) => {
    setCodeFiles(prev => prev.map(file => file.name === fileName ? { ...file, content } : file));
    handleAddLog(`[kernel.c] Flashed modifications to file buffer: ${fileName}`, 'info');
  };

  // Compile code via virtual Clang compiler
  const handleCompileCode = (fileName: string) => {
    handleAddLog(`[clang] Compiling ${fileName} via LLVM engine...`, 'info');
    handleAddLog(`[clang] Target architecture specified: x86_64-none-elf`, 'info');
    handleAddLog(`[lld] Linking compiled object binaries...`, 'info');

    setTimeout(() => {
      handleAddLog(`[lld] Successfully linked hydro_kernel.elf binary. Checksum matches!`, 'success');
      handleAddLog(`[system] Invoking immediate hardware reboot to flash new image...`, 'warning');
      
      // Perform automated reboot sequence
      setTimeout(() => {
        handleReboot();
      }, 1200);
    }, 1500);
  };

  const handleQuickAction = (action: string) => {
    if (action === 'start-all') {
      setContainers(prev => prev.map(c => ({ ...c, status: 'running' })));
      handleAddLog('[db.c] Triggered starting sequence for all registered containers', 'success');
    } else if (action === 'stop-all') {
      setContainers(prev => prev.map(c => ({ ...c, status: 'paused' })));
      handleAddLog('[db.c] Suspended schedules for all registered containers', 'warning');
    } else if (action === 'prune') {
      handleClearContainers();
    }
  };

  return (
    <div className="min-h-screen bg-[#0F0F0F] flex items-center justify-center p-4">
      {/* Absolute bounding box for exactly 1024x768 display layout according to specs */}
      <div className="bg-[#F7F7F5] text-[#37352F] font-sans w-[1024px] h-[768px] flex flex-col overflow-hidden relative border border-[#E9E9E6] shadow-[0_16px_48px_rgba(0,0,0,0.18)] rounded-xl">
        
        {/* BOOT MODE */}
        {(osState === 'booting' || osState === 'rebooting') ? (
          <BootScreen
            onBootComplete={() => setOsState('desktop')}
            rebootTrigger={rebootTrigger}
          />
        ) : (
          /* ACTIVE DESKTOP MODE */
          <>
            {/* Top Menu Bar */}
            <TopMenuBar
              onReboot={handleReboot}
              cpuUsage={cpuUsage}
              ramUsage={ramUsage}
              onOpenWindow={handleOpenWindow}
              onQuickAction={handleQuickAction}
            />

            {/* Main desktop area with windows */}
            <main ref={desktopRef} className="flex-grow relative p-6 overflow-hidden select-none h-[calc(768px-7rem)]">
              {/* Background watermark logo */}
              <div className="absolute inset-0 flex items-center justify-center pointer-events-none opacity-[0.015] select-none">
                <span className="text-[360px] font-bold font-sans">H</span>
              </div>

              {/* Floating Performance widgets */}
              <WidgetPanel cpuUsage={cpuUsage} ramUsage={ramUsage} />

              {/* Window instances */}
              
              {/* Dubnium Docker Manager GUI */}
              {!minimizedWindows['docker-manager'] && (
                <WindowWrapper
                  id="docker-manager"
                  title="Dubnium Docker Manager v1.0.4 - [db.c]"
                  isOpen={openWindows['docker-manager']}
                  onClose={() => handleCloseWindow('docker-manager')}
                  onMinimize={() => handleMinimizeWindow('docker-manager')}
                  zIndex={windowZIndex['docker-manager']}
                  onFocus={() => handleFocusWindow('docker-manager')}
                  defaultX={30}
                  defaultY={30}
                  width="w-[740px]"
                  desktopRef={desktopRef}
                >
                  <DockerManagerWindow
                    containers={containers}
                    onRunContainer={handleRunContainer}
                    onPauseContainer={handlePauseContainer}
                    onStopContainer={handleStopContainer}
                    onRemoveContainer={handleRemoveContainer}
                    onResumeContainer={handleResumeContainer}
                  />
                </WindowWrapper>
              )}

              {/* Clang compiler and code editor */}
              {!minimizedWindows['compiler'] && (
                <WindowWrapper
                  id="compiler"
                  title="Clang target compiler (x86_64-none-elf) - [kernel.c]"
                  isOpen={openWindows['compiler']}
                  onClose={() => handleCloseWindow('compiler')}
                  onMinimize={() => handleMinimizeWindow('compiler')}
                  zIndex={windowZIndex['compiler']}
                  onFocus={() => handleFocusWindow('compiler')}
                  defaultX={160}
                  defaultY={60}
                  width="w-[720px]"
                  desktopRef={desktopRef}
                >
                  <CodeEditorWindow
                    files={codeFiles}
                    onSaveFile={handleSaveFile}
                    onCompile={handleCompileCode}
                  />
                </WindowWrapper>
              )}

              {/* Retro console terminal console */}
              {!minimizedWindows['terminal'] && (
                <WindowWrapper
                  id="terminal"
                  title="HydroOS TTY Terminal (Limine Virtual Console)"
                  isOpen={openWindows['terminal']}
                  onClose={() => handleCloseWindow('terminal')}
                  onMinimize={() => handleMinimizeWindow('terminal')}
                  zIndex={windowZIndex['terminal']}
                  onFocus={() => handleFocusWindow('terminal')}
                  defaultX={220}
                  defaultY={140}
                  width="w-[580px]"
                  desktopRef={desktopRef}
                >
                  <TerminalWindow
                    containers={containers}
                    logs={logs}
                    onAddLog={handleAddLog}
                    onClearLogs={handleClearLogs}
                    onRunContainer={handleRunContainer}
                    onPauseContainer={handlePauseContainer}
                    onStopContainer={handleStopContainer}
                    onTriggerReboot={handleReboot}
                    onCompileCode={handleCompileCode}
                  />
                </WindowWrapper>
              )}

            </main>

            {/* macOS styled Bottom Floating Dock */}
            <Dock
              onOpenWindow={handleOpenWindow}
              activeWindows={openWindows}
              onClearContainers={handleClearContainers}
            />
          </>
        )}
      </div>
    </div>
  );
}
