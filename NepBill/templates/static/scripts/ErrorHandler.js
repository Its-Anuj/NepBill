
export function ClearFieldError(InputId, ErrElemId, InputElemError) {
  const InputElem = document.getElementById(InputId);
  const ErrElem = document.getElementById(ErrElemId);
  if (InputElem) InputElem.classList.remove(InputElemError);
  if (ErrElem) ErrElem.classList.remove("visible");
}

export function ShowFieldError(InputId, ErrElemId, InputElemError, Message) {
  const InputElem = document.getElementById(InputId);
  const ErrElem = document.getElementById(ErrElemId);
  if (InputElem) InputElem.classList.add(InputElemError);
  if (ErrElem) {
    if (Message) ErrElem.textContent = Message;
    ErrElem.classList.add("visible");
  }
}
