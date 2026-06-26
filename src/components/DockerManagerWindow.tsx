import React, { useState } from 'react';
import { Container } from '../types';
import { Database, Server, Terminal as CliIcon, Plus, Play, Pause, Square, Trash, Monitor } from 'lucide-react';

interface DockerManagerWindowProps {
  containers: Container[];
  onRunContainer: (image: string, customPort?: string, customRam?: string) => void;
  onPauseContainer: (id: string) => void;
  onStopContainer: (id: string) => void;
  onRemoveContainer: (id: string) => void;
  onResumeContainer: (id: string) => void;
}

export default function DockerManagerWindow({
  containers,
  onRunContainer,
  onPauseContainer,
  onStopContainer,
  onRemoveContainer,
  onResumeContainer
}: DockerManagerWindowProps) {
  // New Container Form
  const [selectedImage, setSelectedImage] = useState('nginx-stable');
  const [customImage, setCustomImage] = useState('');
  const [customPort, setCustomPort] = useState('8080');
  const [customRam, setCustomRam] = useState('64MB');
  const [isPulling, setIsPulling] = useState(false);
  const [pullProgress, setPullProgress] = useState(0);

  // Active Selected Container for Interactive View Frame
  const [selectedContainerId, setSelectedContainerId] = useState<string | null>(
    containers.length > 0 ? containers[0].id : null
  );

  // Virtual States for Nginx / Postgres / Redis Container Previews
  const [nginxVisitors, setNginxVisitors] = useState(42);
  const [dbRows, setDbRows] = useState([
    { id: 1, name: 'admin_user', role: 'superuser', active: 'TRUE' },
    { id: 2, name: 'docker_agent', role: 'developer', active: 'TRUE' },
    { id: 3, name: 'guest_spec', role: 'readonly', active: 'FALSE' },
  ]);
  const [newDbName, setNewDbName] = useState('');
  const [newDbRole, setNewDbRole] = useState('developer');

  const [redisStore, setRedisStore] = useState<{ [key: string]: string }>({
    'hydro:session': 'active_token_9x1l',
    'oxygen:theme': 'notion_minimalist',
    'db:engine': 'dubnium_v1.0.4',
  });
  const [redisKey, setRedisKey] = useState('');
  const [redisVal, setRedisVal] = useState('');

  const imagesList = [
    { value: 'nginx-stable', label: 'nginx:alpine (Web server)', defaultPort: '80', defaultRam: '45MB' },
    { value: 'postgres-db', label: 'postgres:15 (Relational database)', defaultPort: '5432', defaultRam: '128MB' },
    { value: 'redis-kv', label: 'redis:latest (Key-Value store)', defaultPort: '6379', defaultRam: '16MB' },
    { value: 'alpine-linux', label: 'alpine:latest (BusyBox shell)', defaultPort: 'none', defaultRam: '8MB' },
    { value: 'custom', label: 'Custom Custom Repository...', defaultPort: '8080', defaultRam: '64MB' },
  ];

  const handleImageSelect = (e: React.ChangeEvent<HTMLSelectElement>) => {
    const val = e.target.value;
    setSelectedImage(val);
    const found = imagesList.find(img => img.value === val);
    if (found) {
      setCustomPort(found.defaultPort);
      setCustomRam(found.defaultRam);
    }
  };

  const handleAddContainer = (e: React.FormEvent) => {
    e.preventDefault();
    const finalImage = selectedImage === 'custom' ? (customImage || 'unnamed-repo') : selectedImage;
    
    setIsPulling(true);
    setPullProgress(0);

    // Simulate docker pull streaming logs
    const interval = setInterval(() => {
      setPullProgress(prev => {
        if (prev >= 100) {
          clearInterval(interval);
          setTimeout(() => {
            onRunContainer(finalImage, customPort, customRam);
            setIsPulling(false);
            setCustomImage('');
          }, 300);
          return 100;
        }
        return prev + 25;
      });
    }, 1500 / 4);
  };

  // Find the selected container
  const activeContainer = containers.find(c => c.id === (selectedContainerId || (containers[0]?.id)));

  // Add DB Row inside virtual postgres client
  const handleAddDbRow = (e: React.FormEvent) => {
    e.preventDefault();
    if (!newDbName.trim()) return;
    const newId = dbRows.length + 1;
    setDbRows([
      ...dbRows,
      { id: newId, name: newDbName.trim(), role: newDbRole, active: 'TRUE' }
    ]);
    setNewDbName('');
  };

  // Set Key in virtual Redis client
  const handleSetRedis = (e: React.FormEvent) => {
    e.preventDefault();
    if (!redisKey.trim() || !redisVal.trim()) return;
    setRedisStore({
      ...redisStore,
      [redisKey.trim()]: redisVal.trim()
    });
    setRedisKey('');
    setRedisVal('');
  };

  return (
    <div className="bg-[#FFFFFF] text-[#37352F] font-sans text-[11px] flex flex-col h-[460px] select-text relative">
      {/* Dynamic Pulling Banner Overlay */}
      {isPulling && (
        <div className="absolute inset-0 bg-[#FFFFFF]/90 backdrop-blur-sm z-40 flex flex-col items-center justify-center p-8 text-center">
          <div className="w-12 h-12 bg-[#F7F7F5] text-[#0F7B5C] rounded-xl flex items-center justify-center border border-[#E9E9E6] font-mono font-bold text-lg mb-4 animate-bounce shadow-sm">
            Db
          </div>
          <p className="font-bold text-[#37352F] text-xs mb-1.5 uppercase tracking-wider">Docker Engine Pulling Layer</p>
          <p className="text-[#9B9A97] mb-4 text-[10px]">Downloading checksum blob sha256:7f310a0...</p>
          <div className="w-64 h-[3px] bg-[#E9E9E6] overflow-hidden relative">
            <div className="absolute left-0 top-0 bottom-0 bg-[#0F7B5C] transition-all duration-200" style={{ width: `${pullProgress}%` }} />
          </div>
          <span className="text-[10px] text-[#37352F] mt-2 font-bold font-mono">{pullProgress}% Done</span>
        </div>
      )}

      {/* Primary Panels Layout: Upper List, Bottom Interactive Container Interface */}
      <div className="flex-grow flex flex-col p-4 space-y-4 overflow-y-auto">
        
        {/* UPPER: Container Schedulers List */}
        <div className="bg-[#FFFFFF] border border-[#E9E9E6] rounded-lg p-3 space-y-3 shadow-sm">
          <div className="flex items-center justify-between border-b border-[#E9E9E6] pb-1.5">
            <span className="text-[10px] text-[#37352F] uppercase tracking-wider font-bold flex items-center gap-1">
              <Server className="w-3.5 h-3.5 text-[#0F7B5C]" /> ACTIVE DUBNIUM TASK REGISTER
            </span>
            <span className="text-[10px] bg-[#EDF6F3] text-[#0F7B5C] border border-[#D0E7E1] px-2 py-0.5 rounded-md font-bold font-mono">
              {containers.filter(c => c.status === 'running').length} RUNNING
            </span>
          </div>

          <div className="space-y-1.5">
            {/* Headers */}
            <div className="grid grid-cols-6 border-b border-[#E9E9E6] pb-1.5 text-[#9B9A97] uppercase text-[9px] tracking-wider font-bold font-sans">
              <span>Container ID</span>
              <span>Docker Image</span>
              <span>State</span>
              <span>Port Bind</span>
              <span>Allocated</span>
              <span className="text-right">Scheduler Actions</span>
            </div>

            {/* List */}
            {containers.length === 0 ? (
              <div className="text-center py-4 text-[#9B9A97] font-sans italic">
                No containers running in current Docker process memory.
              </div>
            ) : (
              containers.map((c) => (
                <div
                  key={c.id}
                  onClick={() => setSelectedContainerId(c.id)}
                  className={`grid grid-cols-6 items-center py-1.5 px-2 rounded-md transition-all cursor-pointer ${
                    selectedContainerId === c.id
                      ? 'bg-[#F7F7F5] border border-[#E9E9E6]'
                      : 'hover:bg-[#F7F7F5]/50 border border-transparent'
                  }`}
                >
                  <span className="text-[#D9730D] font-mono font-bold">{c.id}</span>
                  <span className="text-[#37352F] font-bold underline decoration-dotted font-mono">{c.image}</span>
                  <span className="flex items-center gap-1.5">
                    <span className={`w-1.5 h-1.5 rounded-full ${
                      c.status === 'running' ? 'bg-[#0F7B5C] animate-pulse' : c.status === 'paused' ? 'bg-[#D9730D]' : 'bg-[#9B9A97]'
                    }`} />
                    <span className="capitalize font-sans font-medium">{c.status}</span>
                  </span>
                  <span className="font-mono">{c.port === 'none' ? '—' : `:${c.port}`}</span>
                  <span className="text-[#9B9A97] font-mono">{c.memory}</span>
                  
                  {/* Action buttons */}
                  <div className="flex gap-1.5 justify-end" onClick={(e) => e.stopPropagation()}>
                    {c.status === 'running' ? (
                      <button
                        onClick={() => onPauseContainer(c.id)}
                        title="Pause process"
                        className="p-1 hover:bg-[#FFF5EB] border border-[#E9E9E6] hover:border-[#FADCDD] text-[#D9730D] rounded cursor-pointer transition-all bg-[#FFFFFF]"
                      >
                        <Pause className="w-3 h-3" />
                      </button>
                    ) : (
                      <button
                        onClick={() => onResumeContainer(c.id)}
                        title="Resume process"
                        className="p-1 hover:bg-[#EDF6F3] border border-[#E9E9E6] hover:border-[#D0E7E1] text-[#0F7B5C] rounded cursor-pointer transition-all bg-[#FFFFFF]"
                      >
                        <Play className="w-3 h-3" />
                      </button>
                    )}

                    {c.status !== 'stopped' ? (
                      <button
                        onClick={() => onStopContainer(c.id)}
                        title="Terminate process"
                        className="p-1 border border-[#E9E9E6] hover:bg-[#F7F7F5] text-[#37352F] rounded cursor-pointer transition-all bg-[#FFFFFF]"
                      >
                        <Square className="w-3 h-3" />
                      </button>
                    ) : null}

                    <button
                      onClick={() => onRemoveContainer(c.id)}
                      title="Erase container data"
                      className="p-1 hover:bg-[#FFF0F0] border border-[#E9E9E6] hover:border-[#FADCDD] text-[#EB5757] rounded cursor-pointer transition-all bg-[#FFFFFF]"
                    >
                      <Trash className="w-3 h-3" />
                    </button>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>

        {/* BOTTOM DOUBLE PANELS: Form and Active Container Output Frame */}
        <div className="grid grid-cols-1 md:grid-cols-12 gap-4">
          
          {/* Form Panel: Launch New */}
          <div className="md:col-span-5 bg-[#FFFFFF] border border-[#E9E9E6] rounded-lg p-3 space-y-3 flex flex-col justify-between shadow-sm">
            <div>
              <span className="text-[10px] text-[#37352F] uppercase tracking-wider font-bold flex items-center gap-1 border-b border-[#E9E9E6] pb-1.5 mb-2">
                <Plus className="w-3.5 h-3.5 text-[#0F7B5C]" /> DEPLOY REPOSITORY
              </span>

              <form onSubmit={handleAddContainer} className="space-y-2.5">
                <div className="space-y-1">
                  <label className="text-[9px] text-[#9B9A97] uppercase tracking-wider font-semibold">Docker Hub Image</label>
                  <select
                    value={selectedImage}
                    onChange={handleImageSelect}
                    className="w-full bg-[#FFFFFF] border border-[#E9E9E6] rounded px-2.5 py-1 text-[#37352F] font-sans outline-none focus:border-[#0F7B5C] transition-all"
                  >
                    {imagesList.map(img => (
                      <option key={img.value} value={img.value}>{img.label}</option>
                    ))}
                  </select>
                </div>

                {selectedImage === 'custom' && (
                  <div className="space-y-1">
                    <label className="text-[9px] text-[#9B9A97] uppercase tracking-wider font-semibold">Custom Image Repository</label>
                    <input
                      type="text"
                      placeholder="e.g., node:18-alpine"
                      value={customImage}
                      onChange={(e) => setCustomImage(e.target.value)}
                      className="w-full bg-[#FFFFFF] border border-[#E9E9E6] rounded px-2.5 py-1 text-[#37352F] font-mono outline-none focus:border-[#0F7B5C] transition-all"
                    />
                  </div>
                )}

                <div className="grid grid-cols-2 gap-2">
                  <div className="space-y-1">
                    <label className="text-[9px] text-[#9B9A97] uppercase tracking-wider font-semibold">Port Binding</label>
                    <input
                      type="text"
                      value={customPort}
                      onChange={(e) => setCustomPort(e.target.value)}
                      className="w-full bg-[#FFFFFF] border border-[#E9E9E6] rounded px-2.5 py-1 text-[#37352F] font-mono outline-none"
                    />
                  </div>
                  <div className="space-y-1">
                    <label className="text-[9px] text-[#9B9A97] uppercase tracking-wider font-semibold">RAM Limit</label>
                    <input
                      type="text"
                      value={customRam}
                      onChange={(e) => setCustomRam(e.target.value)}
                      className="w-full bg-[#FFFFFF] border border-[#E9E9E6] rounded px-2.5 py-1 text-[#37352F] font-mono outline-none"
                    />
                  </div>
                </div>

                <button
                  type="submit"
                  className="w-full mt-2 py-1.5 bg-[#0F7B5C] hover:bg-[#0C6249] text-white font-bold rounded flex items-center justify-center gap-1.5 cursor-pointer shadow-sm transition-all uppercase text-[10px]"
                >
                  <Plus className="w-3.5 h-3.5" /> Deploy Container
                </button>
              </form>
            </div>
          </div>

          {/* Virtual Output Frame (Fulfillment of macOS visual and functional sandbox requirement!) */}
          <div className="md:col-span-7 bg-[#FFFFFF] border border-[#E9E9E6] rounded-lg overflow-hidden flex flex-col justify-between shadow-sm h-[190px]">
            
            {/* Output Frame Header */}
            <div className="bg-[#F7F7F5] h-6 px-3 flex items-center justify-between border-b border-[#E9E9E6] select-none">
              <div className="flex items-center gap-1.5 text-[#37352F]">
                <Monitor className="w-3.5 h-3.5 text-[#0F7B5C]" />
                <span className="text-[9px] font-bold uppercase tracking-wider text-[#37352F]">
                  Container Sandbox Output UI
                </span>
              </div>
              <div className="flex items-center gap-1">
                <span className="w-1.5 h-1.5 rounded-full bg-[#EB5757]/60" />
                <span className="w-1.5 h-1.5 rounded-full bg-[#F2C94C]/60" />
                <span className="w-1.5 h-1.5 rounded-full bg-[#27AE60]/60" />
              </div>
            </div>

            {/* Output Content */}
            <div className="flex-grow p-3 overflow-y-auto bg-[#FFFFFF] text-[#37352F] font-mono">
              {!activeContainer ? (
                <div className="h-full flex flex-col items-center justify-center text-[#9B9A97] italic font-sans">
                  Select an active container above to stream its output interface.
                </div>
              ) : activeContainer.status !== 'running' ? (
                <div className="h-full flex flex-col items-center justify-center text-[#D9730D] text-center p-4 font-sans">
                  <span className="text-[18px] mb-1">⏸</span>
                  <p className="font-bold">Container process is suspended</p>
                  <p className="text-[10px] text-[#9B9A97] mt-1">Oxygen virtual framebuffer is suspended for {activeContainer.id}. Resume process to reload.</p>
                </div>
              ) : (
                <div className="h-full">
                  {/* Conditional Render Based on Container Category/Name */}
                  {activeContainer.image.includes('nginx') ? (
                    <div className="space-y-2 h-full flex flex-col justify-between">
                      <div className="border border-[#D0E7E1] bg-[#EDF6F3] rounded p-2 text-[10px] space-y-1">
                        <div className="flex justify-between items-center border-b border-[#D0E7E1] pb-1">
                          <span className="text-[#0F7B5C] font-bold">Nginx Server active</span>
                          <span className="text-[#9B9A97] font-sans">Bind Port 80</span>
                        </div>
                        <p className="text-[#37352F] font-bold">Welcome to nginx inside HydroOS!</p>
                        <p className="text-[#9B9A97] font-sans">Serving compiled static index.html via fast mapped Oxygen filesystem buffer.</p>
                      </div>

                      <div className="flex items-center justify-between text-[10px] font-sans">
                        <span className="text-[#9B9A97]">Live Visitor Count: <strong className="text-[#37352F]">{nginxVisitors}</strong></span>
                        <button
                          onClick={() => setNginxVisitors(p => p + 1)}
                          className="px-2.5 py-1 bg-[#F7F7F5] border border-[#E9E9E6] hover:bg-[#EFEFED] rounded text-[#37352F] cursor-pointer transition-all font-medium"
                        >
                          Ping Request
                        </button>
                      </div>
                    </div>
                  ) : activeContainer.image.includes('postgres') ? (
                    <div className="space-y-2 text-[9px] h-full flex flex-col justify-between font-sans">
                      <div className="overflow-x-auto">
                        <table className="w-full text-left">
                          <thead>
                            <tr className="border-b border-[#E9E9E6] text-[#9B9A97] font-bold">
                              <th className="py-1">ID</th>
                              <th>USERNAME</th>
                              <th>ROLE</th>
                              <th>ACTIVE</th>
                            </tr>
                          </thead>
                          <tbody>
                            {dbRows.map((row) => (
                              <tr key={row.id} className="border-b border-[#F7F7F5]">
                                <td className="py-0.5 text-[#9B9A97] font-mono">{row.id}</td>
                                <td className="text-[#37352F] font-bold font-mono">{row.name}</td>
                                <td className="text-[#D9730D] font-mono">{row.role}</td>
                                <td className={row.active === 'TRUE' ? 'text-[#0F7B5C] font-bold' : 'text-[#9B9A97]'}>{row.active}</td>
                              </tr>
                            ))}
                          </tbody>
                        </table>
                      </div>

                      {/* Add DB Row Form */}
                      <form onSubmit={handleAddDbRow} className="flex gap-2 items-center border-t border-[#E9E9E6] pt-1.5">
                        <input
                          type="text"
                          placeholder="username"
                          value={newDbName}
                          onChange={(e) => setNewDbName(e.target.value)}
                          className="flex-grow bg-[#FFFFFF] border border-[#E9E9E6] rounded px-2 py-0.5 text-[#37352F] outline-none font-mono"
                        />
                        <select
                          value={newDbRole}
                          onChange={(e) => setNewDbRole(e.target.value)}
                          className="bg-[#FFFFFF] border border-[#E9E9E6] rounded px-1.5 py-0.5 text-[#37352F] outline-none"
                        >
                          <option value="developer">developer</option>
                          <option value="moderator">moderator</option>
                          <option value="analyst">analyst</option>
                        </select>
                        <button type="submit" className="px-2 py-0.5 bg-[#0F7B5C] hover:bg-[#0C6249] rounded text-white font-bold cursor-pointer text-[9.5px]">
                          Insert
                        </button>
                      </form>
                    </div>
                  ) : activeContainer.image.includes('redis') ? (
                    <div className="space-y-2 h-full flex flex-col justify-between text-[10px] font-sans">
                      {/* Redis Keys Store Scroll list */}
                      <div className="flex-1 overflow-y-auto space-y-1 bg-[#F7F7F5] p-1.5 rounded border border-[#E9E9E6] text-[9.5px] max-h-[85px] font-mono">
                        {Object.entries(redisStore).map(([k, v]) => (
                          <div key={k} className="flex justify-between border-b border-[#E9E9E6] py-0.5">
                            <span className="text-[#EB5757] font-bold">{k}</span>
                            <span className="text-[#9B9A97]">"{v}"</span>
                          </div>
                        ))}
                      </div>

                      {/* Add Redis Key Value */}
                      <form onSubmit={handleSetRedis} className="flex gap-2 items-center">
                        <input
                          type="text"
                          placeholder="key"
                          value={redisKey}
                          onChange={(e) => setRedisKey(e.target.value)}
                          className="w-1/3 bg-[#FFFFFF] border border-[#E9E9E6] rounded px-1.5 py-0.5 text-[#37352F] outline-none text-[9px] font-mono"
                        />
                        <input
                          type="text"
                          placeholder="value string"
                          value={redisVal}
                          onChange={(e) => setRedisVal(e.target.value)}
                          className="flex-grow bg-[#FFFFFF] border border-[#E9E9E6] rounded px-1.5 py-0.5 text-[#37352F] outline-none text-[9px] font-mono"
                        />
                        <button type="submit" className="px-2 py-0.5 bg-[#EB5757] hover:bg-[#C0392B] rounded text-white font-bold cursor-pointer text-[9px]">
                          SET
                        </button>
                      </form>
                    </div>
                  ) : (
                    <div className="h-full flex flex-col justify-between text-[10px]">
                      <div className="bg-[#F7F7F5] border border-[#E9E9E6] p-2 rounded text-[10px] space-y-1 font-mono">
                        <p className="text-[#9B9A97]">alpine-linux-sh (tty1)</p>
                        <p className="text-[#0F7B5C] font-semibold">$ uname -a</p>
                        <p className="text-[#37352F]">Linux hydro-alpine 5.10.0-none-elf x86_64 GNU/Linux</p>
                        <p className="text-[#0F7B5C] font-semibold">$ whoami</p>
                        <p className="text-[#37352F]">root</p>
                      </div>
                      <div className="text-[#9B9A97] text-[9px] italic text-right font-sans">
                        Simulating sandbox tty for {activeContainer.image}
                      </div>
                    </div>
                  )}
                </div>
              )}
            </div>

            {/* Footer details */}
            {activeContainer && activeContainer.status === 'running' && (
              <div className="bg-[#F7F7F5] h-5 px-3 flex justify-between items-center text-[9px] text-[#9B9A97] border-t border-[#E9E9E6]">
                <span>RESOURCES: CPU {activeContainer.cpu}% • NET {activeContainer.port === 'none' ? 'CLOSED' : '0.0.0.0:' + activeContainer.port}</span>
                <span className="text-[#0F7B5C] uppercase font-bold animate-pulse font-sans">● Interactive Live Output</span>
              </div>
            )}
          </div>

        </div>

      </div>
    </div>
  );
}
