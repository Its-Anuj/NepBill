export const PurchaseOrderStateMap = {
  Draft: 0,
  Sent: 1,
  PartiallyReceived: 2,
  Completed: 3,
  Cancelled: 4,
};

export function PurchaseOrderStateToStr(state) {
    console.log(state);
  switch (state) {
    case 0 : {return "draft";}
    case 1 : {return "sent";}
    case 2 : {return "partiallyreceived";}
    case 3 : {return "completed";}
    case 4 : {return "cancelled";}
  }
}
