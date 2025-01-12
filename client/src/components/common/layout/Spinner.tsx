import { Loader2 } from "lucide-react";

type Size = "small" | "default" | "large";

export default function LoadingSpinner({ size = "default" }: { size?: Size }) {
  const sizeClasses = {
    small: "w-4 h-4",
    default: "w-8 h-8",
    large: "w-12 h-12",
  };

  return (
    <div className="flex justify-center items-center">
      <Loader2 className={`${sizeClasses[size]} animate-spin`} />
    </div>
  );
}
